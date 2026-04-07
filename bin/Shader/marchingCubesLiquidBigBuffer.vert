#version 460 core

out vec2 texCoord;
out vec3 vertexTextureWeights;
out mat3 TBN;
flat out uvec3 triangleVertexIds;

layout(std140, binding = 0) uniform DrawVars
{
    mat4 projectionView;
    mat4 projection;
    mat4 view;
};

layout(std430, binding = 11) readonly buffer ChunkVoxelGrid { uint voxelGrid[]; }; // src/Voxel/MarchingCubesManager.h::SSBO_BLOCK__GLOBAL_CHUNK_GRIDS_STORAGE

layout(std430, binding = 12) readonly buffer ChunkData { uint packedData[]; }; // src/Voxel/MarchingCubesManager.h::SSBO_BLOCK__GLOBAL_CHUNK_STORAGE

layout(std430, binding = 13) readonly buffer ChunkPositions { ivec4 chunkPositions[]; }; // src/Voxel/MarchingCubesManager.h::SSBO_BLOCK__CHUNK_POSITIONS

struct TriangleVariant {
    vec4 pos_TBN_tex[6];
};
struct TriangleData {
    TriangleVariant variants[512];
    uvec4 vertexVoxelIds;
};
layout(std430, binding = 14) readonly buffer Verteces { TriangleData triangleData[]; }; // src/Voxel/MarchingCubesManager.h::SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS

layout(std430, binding = 15) readonly buffer ChunkRefs { int chunkRefs[]; }; // src/Voxel/MarchingCubesManager.h::SSBO_BLOCK__DRAW_ID_TO_CHUNK

struct UnpackedData {
    uint x;
    uint y;
    uint z;
};
struct UnpackedVoxel {
    uint id;
    uint size;
    bool haveSolid;
};

const vec3 triangleVertexTextureWeights[3] = { vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)};
const uvec3 idOffset[8] = {
    uvec3(0, 1, 1), uvec3(1, 1, 1), uvec3(1, 1, 0), uvec3(0, 1, 0),
    uvec3(0, 0, 1), uvec3(1, 0, 1), uvec3(1, 0, 0), uvec3(0, 0, 0)
};
// Voxel grid sizes
const uint gridDimensionSize = 32;
const uint gridSliceSize = gridDimensionSize * gridDimensionSize;
const uint gridChunkSize = gridDimensionSize * gridDimensionSize * gridDimensionSize;
// Packed data sizes
const uint dataDimensionSize = 31;
const uint dataSliceSize = dataDimensionSize * dataDimensionSize;
const uint dataChunkSize = dataDimensionSize * dataDimensionSize * dataDimensionSize;
// Vertex position offsets count
const uint maxOffset = 8;
const uint twoOffsetCombinations = maxOffset * maxOffset;

UnpackedData unpackData(uint data);
UnpackedVoxel unpackVoxel(uint raw);
UnpackedVoxel[8] unpackCubeVertices(UnpackedData data, uint chunkId);
vec3 adjustBlendValues(uint curVertex, uint v0Id, uint v1Id, uint v2Id);

void main()
{
    int currentInstance = gl_BaseInstance + gl_InstanceID;
    uint chunkId = chunkRefs[gl_DrawID];
    uint dataLocation = (chunkId * dataChunkSize) + currentInstance;
    UnpackedData data = unpackData(packedData[dataLocation]); 

    int localTriangleVertexId = gl_VertexID % 3;
    int globalTriangleBaseVertexId = (gl_VertexID - localTriangleVertexId);
    int globalTriangleId = (globalTriangleBaseVertexId - gl_BaseVertex) / 3;

    uint v0Id = triangleData[globalTriangleId].vertexVoxelIds[0];
    uint v1Id = triangleData[globalTriangleId].vertexVoxelIds[1];
    uint v2Id = triangleData[globalTriangleId].vertexVoxelIds[2];
    
    UnpackedVoxel cube[8] = unpackCubeVertices(data, chunkId);

    bool v0IsDirDown = (triangleData[globalTriangleId].vertexVoxelIds[3] & 1) != 0;
    bool v1IsDirDown = (triangleData[globalTriangleId].vertexVoxelIds[3] & 2) != 0;
    bool v2IsDirDown = (triangleData[globalTriangleId].vertexVoxelIds[3] & 4) != 0;

    bool v0IsInvalid = v0IsDirDown && cube[v0Id].haveSolid;
    bool v1IsInvalid = v1IsDirDown && cube[v1Id].haveSolid;
    bool v2IsInvalid = v2IsDirDown && cube[v2Id].haveSolid;
    float invalidDiscard = (v0IsInvalid && v1IsInvalid && v2IsInvalid) ? 0.0 : 1.0;

    uint vertexOffsetX = (v0IsInvalid) ? 0 : cube[v0Id].size;
    uint vertexOffsetY = (v1IsInvalid) ? 0 : cube[v1Id].size;
    uint vertexOffsetZ = (v2IsInvalid) ? 0 : cube[v2Id].size;

    uint variantId = (vertexOffsetZ * twoOffsetCombinations) + (vertexOffsetY * maxOffset) + vertexOffsetX;
    vec3 chunkPosition = chunkPositions[chunkId].xyz;

    vec4 localPosition = vec4(
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[localTriangleVertexId].x + chunkPosition.x + data.x,
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[localTriangleVertexId].y + chunkPosition.y + data.y,
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[localTriangleVertexId].z + chunkPosition.z + data.z, 1.0);
    gl_Position = (projectionView * localPosition) * invalidDiscard;

    TBN = mat3(
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[3].xyz,
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[4].xyz,
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[5].xyz
    );

    uint texId = localTriangleVertexId * 2;
    texCoord = vec2(
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[texId].w,
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[texId + 1].w
    );

    // При объединении слоёв для жидкостей и твердых вокселей у итоговой фигуры слоя жидкостей
    // появляется проблема, когда вершины фигуры принадлежат вершинам куба с пустым значением 
    // id для жидкости. В таком случае для вершины выбирается некорректная чёрная текстура.
    // Существует 3 варианта данной проблемы - когда в треугольнике отсутствует id у 1, 2 или
    // в худшем случае всех 3-х вершин.
    //
    // Следующий блок кода решает худший случай, когда отсутсвуют id для всех 3-х вершин.
    // Данный случай возникает, когда при объединении в кубе присутствует одна вершина с жидкостью
    // или таких вершин 2, но вторая вершина с водой лежит в противоположном от первой вершины углу куба.
    // Ещё в случае с 2 вершинами, эти вершины принадлежат двум несвязанным друг с другом группам 
    // треугольников (Такие группы не соединяются в единую, неприрывную фигуру).
    // В остальных случаях если вершины c отсутствующим id и присутствуют, то треугольники, которым
    // они принадлежат, скрыты за геометрией фигуры твёрдого слоя и следовательно невидимы.
    //
    // В соотвествии с особенностями данная проблема решается поиском первой попавшейся вершины с 
    // наличием id путём обхода всех вершин куба относительно позиции одной из вершин треугольника 
    // (в данном случае берётся вершина v0Id) в следующем порядке:
    // 1. Паралельные осям соседи. 2. Диагональные по плоскостям вершины. 3. Противоположный угол куба.
    // После данной операции проблема переходит в случай отсутствия 2 вершин, что уже решается в 
    // функции adjustBlendValues().
    if ((cube[v0Id].id == 255u) && (cube[v1Id].id == 255u) && (cube[v2Id].id == 255u)) {
        const int replaceMap[56] = {
            1, 3, 4, 2, 5, 7, 6, // v0Id == 0
            0, 2, 5, 3, 4, 6, 7, // v0Id == 1
            1, 3, 6, 0, 5, 7, 4, // v0Id == 2
            0, 2, 7, 1, 4, 6, 5, // v0Id == 3
            5, 7, 0, 6, 1, 3, 2, // v0Id == 4
            4, 6, 1, 7, 0, 2, 3, // v0Id == 5
            5, 7, 2, 4, 3, 1, 0, // v0Id == 6
            4, 6, 3, 5, 0, 2, 1  // v0Id == 7
        };

        uint rId = v0Id * 7;
        if (cube[replaceMap[rId]].id != 255u) cube[v0Id].id = cube[replaceMap[rId]].id;
        else if (cube[replaceMap[rId + 1]].id != 255u) cube[v0Id].id = cube[replaceMap[rId + 1]].id;
        else if (cube[replaceMap[rId + 2]].id != 255u) cube[v0Id].id = cube[replaceMap[rId + 2]].id;
        else if (cube[replaceMap[rId + 3]].id != 255u) cube[v0Id].id = cube[replaceMap[rId + 3]].id;
        else if (cube[replaceMap[rId + 4]].id != 255u) cube[v0Id].id = cube[replaceMap[rId + 4]].id;
        else if (cube[replaceMap[rId + 5]].id != 255u) cube[v0Id].id = cube[replaceMap[rId + 5]].id;
        else cube[v0Id].id = cube[replaceMap[rId + 6]].id;
    } 

    vertexTextureWeights = adjustBlendValues(localTriangleVertexId, cube[v0Id].id, cube[v1Id].id, cube[v2Id].id);

    triangleVertexIds.x = cube[v0Id].id;
    triangleVertexIds.y = cube[v1Id].id;
    triangleVertexIds.z = cube[v2Id].id;
}

UnpackedData unpackData(uint data)
{
    UnpackedData unpaked;

    unpaked.x = (data >> 25) & 31; // 31 = 0b11111
    unpaked.y = (data >> 20) & 31; // 31 = 0b11111
    unpaked.z = (data >> 15) & 31; // 31 = 0b11111

    return unpaked;
}

UnpackedVoxel unpackVoxel(uint raw)
{
    UnpackedVoxel unpacked;

    unpacked.id = (raw >> 16) & 255; // 255 = 0bFF
    unpacked.size = (raw >> 24) & 7; // 7 = 0b111
    unpacked.haveSolid = (raw & 255) != 255;

    return unpacked;
}

UnpackedVoxel[8] unpackCubeVertices(UnpackedData data, uint chunkId) {
    uint v7FlatArrayId = (data.z * gridSliceSize) + (data.y * gridDimensionSize) + (data.x);
    uint v6FlatArrayId = v7FlatArrayId + 1;
    uint v4FlatArrayId = v7FlatArrayId + gridSliceSize;
    uint v5FlatArrayId = v4FlatArrayId + 1;

    uint v3FlatArrayId = v7FlatArrayId + gridDimensionSize;
    uint v2FlatArrayId = v6FlatArrayId + gridDimensionSize;
    uint v0FlatArrayId = v4FlatArrayId + gridDimensionSize;
    uint v1FlatArrayId = v5FlatArrayId + gridDimensionSize;

    uint gridLocation = chunkId * gridChunkSize;

    UnpackedVoxel result[8] = {
        unpackVoxel(voxelGrid[gridLocation + v0FlatArrayId]),
        unpackVoxel(voxelGrid[gridLocation + v1FlatArrayId]),
        unpackVoxel(voxelGrid[gridLocation + v2FlatArrayId]),
        unpackVoxel(voxelGrid[gridLocation + v3FlatArrayId]),
        unpackVoxel(voxelGrid[gridLocation + v4FlatArrayId]),
        unpackVoxel(voxelGrid[gridLocation + v5FlatArrayId]),
        unpackVoxel(voxelGrid[gridLocation + v6FlatArrayId]),
        unpackVoxel(voxelGrid[gridLocation + v7FlatArrayId])
    };
    return result;
}

vec3 adjustBlendValues(uint curVertex, uint v0Id, uint v1Id, uint v2Id) {
    uint vertexId[3] = { v0Id, v1Id, v2Id };
    vec3 masks = vec3(
        float(v0Id != 255u),
        float(v1Id != 255u), 
        float(v2Id != 255u)
    );

    float sum = masks.x + masks.y + masks.z;
    vec3 normalizedMasks = masks / max(sum, 1e-6);

    float isCurrentInvalid = float(vertexId[curVertex] == 255u);
    vec3 weight = triangleVertexTextureWeights[curVertex];
    weight = mix(weight, normalizedMasks, isCurrentInvalid);

    return weight;
}
