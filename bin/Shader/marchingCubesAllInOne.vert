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

layout(std430, binding = 12) readonly buffer ChunkData { uvec2 packedData[]; }; // src/Voxel/MarchingCubesManager.h::SSBO_BLOCK__GLOBAL_CHUNK_STORAGE

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
    uint offsets[6];
    uint textureIds[6];
};
struct UnpackedVoxel {
    uint id;
    uint size;
};

const vec3 triangleVertexTextureWeights[3] = { vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)};
const uvec3 idOffset[8] = {
    uvec3(0, 1, 1), uvec3(1, 1, 1), uvec3(1, 1, 0), uvec3(0, 1, 0),
    uvec3(0, 0, 1), uvec3(1, 0, 1), uvec3(1, 0, 0), uvec3(0, 0, 0)
};

UnpackedData unpackData(uvec2 data);
UnpackedVoxel unpackVoxel(uint raw);

void main()
{
    int currentInstance = gl_BaseInstance + gl_InstanceID;
    uint chunkId = chunkRefs[gl_DrawID];
    uint dataLocation = (chunkId * 32768) + currentInstance; // 32768 = 32 * 32 * 32
    UnpackedData data = unpackData(packedData[dataLocation]); 

    int localTriangleVertexId = gl_VertexID % 3;
    int globalTriangleBaseVertexId = (gl_VertexID - localTriangleVertexId);
    int globalTriangleId = (globalTriangleBaseVertexId - gl_BaseVertex) / 3;

    uint v0Id = triangleData[globalTriangleId].vertexVoxelIds[0];
    uint v1Id = triangleData[globalTriangleId].vertexVoxelIds[1];
    uint v2Id = triangleData[globalTriangleId].vertexVoxelIds[2];

    // 33 * 33 = 1089
    uint v0FlatArrayId = ((data.z + idOffset[v0Id].z) * 1089) + ((data.y + idOffset[v0Id].y) * 33) + (data.x + idOffset[v0Id].x);
    uint v1FlatArrayId = ((data.z + idOffset[v1Id].z) * 1089) + ((data.y + idOffset[v1Id].y) * 33) + (data.x + idOffset[v1Id].x);
    uint v2FlatArrayId = ((data.z + idOffset[v2Id].z) * 1089) + ((data.y + idOffset[v2Id].y) * 33) + (data.x + idOffset[v2Id].x);

    uint gridLocation = chunkId * 35937; // 35937 = 33 * 33 * 33
    uint v0Raw = voxelGrid[gridLocation + v0FlatArrayId];
    uint v1Raw = voxelGrid[gridLocation + v1FlatArrayId];
    uint v2Raw = voxelGrid[gridLocation + v2FlatArrayId];

    UnpackedVoxel v0 = unpackVoxel(v0Raw);
    UnpackedVoxel v1 = unpackVoxel(v1Raw);
    UnpackedVoxel v2 = unpackVoxel(v2Raw);

    uint vertexOffsetX = v0.size;
    uint vertexOffsetY = v1.size;
    uint vertexOffsetZ = v2.size;

    // 8 * 8 = 64
    uint variantId = vertexOffsetZ * 64 + vertexOffsetY * 8 + vertexOffsetX;
    vec3 chunkPosition = chunkPositions[chunkId].xyz;

    vec4 localPosition = vec4(
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[localTriangleVertexId].x + chunkPosition.x + data.x,
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[localTriangleVertexId].y + chunkPosition.y + data.y,
        triangleData[globalTriangleId].variants[variantId].pos_TBN_tex[localTriangleVertexId].z + chunkPosition.z + data.z, 1.0);
    gl_Position = projectionView * localPosition;

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
    vertexTextureWeights = triangleVertexTextureWeights[localTriangleVertexId];

    triangleVertexIds.x = v0.id;
    triangleVertexIds.y = v1.id;
    triangleVertexIds.z = v2.id;
}

UnpackedData unpackData(uvec2 data)
{
    UnpackedData unpaked;

    unpaked.x = (data[0] >> 25) & 31; // 31 = 0b11111
    unpaked.y = (data[0] >> 20) & 31; // 31 = 0b11111
    unpaked.z = (data[0] >> 15) & 31; // 31 = 0b11111
    unpaked.offsets[0] = (data[0] >> 12) & 7; // 7 = 0b111;
    unpaked.offsets[1] = (data[0] >> 9) & 7; // 7 = 0b111;
    unpaked.offsets[2] = (data[0] >> 6) & 7; // 7 = 0b111;
    unpaked.offsets[3] = (data[0] >> 3) & 7; // 7 = 0b111;
    unpaked.offsets[4] = data[0] & 7; // 7 = 0b111;

    unpaked.offsets[5] = (data.y >> 28) & 7; // 7 = 0b111;
    unpaked.textureIds[0] = (data.y >> 21) & 127; // 127 = 0b1111111;
    unpaked.textureIds[1] = (data.y >> 14) & 127; // 127 = 0b1111111;
    unpaked.textureIds[2] = (data.y >> 7) & 127; // 127 = 0b1111111;
    unpaked.textureIds[3] = data.y & 127; // 127 = 0b1111111

    unpaked.textureIds[4] = unpaked.textureIds[2];
    unpaked.textureIds[5] = unpaked.textureIds[3];

    return unpaked;
}

UnpackedVoxel unpackVoxel(uint raw)
{
    UnpackedVoxel unpacked;
    unpacked.id = raw & 255; // 255 = 0bFF
    unpacked.size = (raw >> 8) & 7; // 7 = 0b111

    return unpacked;
}
