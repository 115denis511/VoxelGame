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

layout(std140, binding = 2) uniform ChunkPositions
{
    vec4 chunkPositions[14];
};

layout(std430, binding = 0) readonly buffer ChunkData_0 { uint packedData_0[]; };
layout(std430, binding = 1) readonly buffer ChunkVoxelGrid_1 { uint voxelGrid_1[]; };
layout(std430, binding = 2) readonly buffer ChunkData_2 { uint packedData_2[]; };
layout(std430, binding = 3) readonly buffer ChunkVoxelGrid_3 { uint voxelGrid_3[]; };
layout(std430, binding = 4) readonly buffer ChunkData_4 { uint packedData_4[]; };
layout(std430, binding = 5) readonly buffer ChunkVoxelGrid_5 { uint voxelGrid_5[]; };
layout(std430, binding = 6) readonly buffer ChunkData_6 { uint packedData_6[]; };
layout(std430, binding = 7) readonly buffer ChunkVoxelGrid_7 { uint voxelGrid_7[]; };
layout(std430, binding = 8) readonly buffer ChunkData_8 { uint packedData_8[]; };
layout(std430, binding = 9) readonly buffer ChunkVoxelGrid_9 { uint voxelGrid_9[]; };
layout(std430, binding = 10) readonly buffer ChunkData_10 { uint packedData_10[]; };
layout(std430, binding = 11) readonly buffer ChunkVoxelGrid_11 { uint voxelGrid_11[]; };
layout(std430, binding = 12) readonly buffer ChunkData_12 { uint packedData_12[]; };
layout(std430, binding = 13) readonly buffer ChunkVoxelGrid_13 { uint voxelGrid_13[]; };

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
const uvec2 replaceMap[8] = {
    uvec2(1, 3), uvec2(0, 2), uvec2(1, 3), uvec2(0, 2),
    uvec2(5, 7), uvec2(4, 6), uvec2(5, 7), uvec2(4, 6)
};

UnpackedData unpackData(uint data);
UnpackedVoxel unpackVoxel(uint raw);
vec3 adjustBlendValues(uint curVertex, uint v0Id, uint v1Id, uint v2Id);

void main()
{
    int currentInstance = gl_BaseInstance + gl_InstanceID;
    uint chunkBatchId = chunkRefs[gl_DrawID];

    int localTriangleVertexId = gl_VertexID % 3;
    int globalTriangleBaseVertexId = (gl_VertexID - localTriangleVertexId);
    int globalTriangleId = (globalTriangleBaseVertexId - gl_BaseVertex) / 3;

    uint v0Id = triangleData[globalTriangleId].vertexVoxelIds[0];
    uint v1Id = triangleData[globalTriangleId].vertexVoxelIds[1];
    uint v2Id = triangleData[globalTriangleId].vertexVoxelIds[2];

    UnpackedData data;
    switch (chunkBatchId) {
        case 0:
            data = unpackData(packedData_0[currentInstance]);
            break;
        case 1:
            data = unpackData(packedData_2[currentInstance]);
            break;
        case 2:
            data = unpackData(packedData_4[currentInstance]);
            break;
        case 3:
            data = unpackData(packedData_6[currentInstance]);
            break;
        case 4:
            data = unpackData(packedData_8[currentInstance]);
            break;
        case 5:
            data = unpackData(packedData_10[currentInstance]);
            break;
        case 6:
            data = unpackData(packedData_12[currentInstance]);
            break;
    }

    // 33 * 33 = 1089
    uint v7FlatArrayId = (data.z * 1089) + (data.y * 33) + (data.x);
    uint v6FlatArrayId = v7FlatArrayId + 1;
    uint v4FlatArrayId = v7FlatArrayId + 1089;
    uint v5FlatArrayId = v4FlatArrayId + 1;

    uint v3FlatArrayId = v7FlatArrayId + 33;
    uint v2FlatArrayId = v6FlatArrayId + 33;
    uint v0FlatArrayId = v4FlatArrayId + 33;
    uint v1FlatArrayId = v5FlatArrayId + 33;

    UnpackedVoxel cube[8];
    switch (chunkBatchId) {
        case 0:
            cube = UnpackedVoxel[8](
                unpackVoxel(voxelGrid_1[v0FlatArrayId]),
                unpackVoxel(voxelGrid_1[v1FlatArrayId]),
                unpackVoxel(voxelGrid_1[v2FlatArrayId]),
                unpackVoxel(voxelGrid_1[v3FlatArrayId]),
                unpackVoxel(voxelGrid_1[v4FlatArrayId]),
                unpackVoxel(voxelGrid_1[v5FlatArrayId]),
                unpackVoxel(voxelGrid_1[v6FlatArrayId]),
                unpackVoxel(voxelGrid_1[v7FlatArrayId])
            );
            break;
        case 1:
            cube = UnpackedVoxel[8](
                unpackVoxel(voxelGrid_3[v0FlatArrayId]),
                unpackVoxel(voxelGrid_3[v1FlatArrayId]),
                unpackVoxel(voxelGrid_3[v2FlatArrayId]),
                unpackVoxel(voxelGrid_3[v3FlatArrayId]),
                unpackVoxel(voxelGrid_3[v4FlatArrayId]),
                unpackVoxel(voxelGrid_3[v5FlatArrayId]),
                unpackVoxel(voxelGrid_3[v6FlatArrayId]),
                unpackVoxel(voxelGrid_3[v7FlatArrayId])
            );
            break;
        case 2:
            cube = UnpackedVoxel[8](
                unpackVoxel(voxelGrid_5[v0FlatArrayId]),
                unpackVoxel(voxelGrid_5[v1FlatArrayId]),
                unpackVoxel(voxelGrid_5[v2FlatArrayId]),
                unpackVoxel(voxelGrid_5[v3FlatArrayId]),
                unpackVoxel(voxelGrid_5[v4FlatArrayId]),
                unpackVoxel(voxelGrid_5[v5FlatArrayId]),
                unpackVoxel(voxelGrid_5[v6FlatArrayId]),
                unpackVoxel(voxelGrid_5[v7FlatArrayId])
            );
            break;
        case 3:
            cube = UnpackedVoxel[8](
                unpackVoxel(voxelGrid_7[v0FlatArrayId]),
                unpackVoxel(voxelGrid_7[v1FlatArrayId]),
                unpackVoxel(voxelGrid_7[v2FlatArrayId]),
                unpackVoxel(voxelGrid_7[v3FlatArrayId]),
                unpackVoxel(voxelGrid_7[v4FlatArrayId]),
                unpackVoxel(voxelGrid_7[v5FlatArrayId]),
                unpackVoxel(voxelGrid_7[v6FlatArrayId]),
                unpackVoxel(voxelGrid_7[v7FlatArrayId])
            );
            break;
        case 4:
            cube = UnpackedVoxel[8](
                unpackVoxel(voxelGrid_9[v0FlatArrayId]),
                unpackVoxel(voxelGrid_9[v1FlatArrayId]),
                unpackVoxel(voxelGrid_9[v2FlatArrayId]),
                unpackVoxel(voxelGrid_9[v3FlatArrayId]),
                unpackVoxel(voxelGrid_9[v4FlatArrayId]),
                unpackVoxel(voxelGrid_9[v5FlatArrayId]),
                unpackVoxel(voxelGrid_9[v6FlatArrayId]),
                unpackVoxel(voxelGrid_9[v7FlatArrayId])
            );
            break;
        case 5:
            cube = UnpackedVoxel[8](
                unpackVoxel(voxelGrid_11[v0FlatArrayId]),
                unpackVoxel(voxelGrid_11[v1FlatArrayId]),
                unpackVoxel(voxelGrid_11[v2FlatArrayId]),
                unpackVoxel(voxelGrid_11[v3FlatArrayId]),
                unpackVoxel(voxelGrid_11[v4FlatArrayId]),
                unpackVoxel(voxelGrid_11[v5FlatArrayId]),
                unpackVoxel(voxelGrid_11[v6FlatArrayId]),
                unpackVoxel(voxelGrid_11[v7FlatArrayId])
            );
            break;
        case 6:
            cube = UnpackedVoxel[8](
                unpackVoxel(voxelGrid_13[v0FlatArrayId]),
                unpackVoxel(voxelGrid_13[v1FlatArrayId]),
                unpackVoxel(voxelGrid_13[v2FlatArrayId]),
                unpackVoxel(voxelGrid_13[v3FlatArrayId]),
                unpackVoxel(voxelGrid_13[v4FlatArrayId]),
                unpackVoxel(voxelGrid_13[v5FlatArrayId]),
                unpackVoxel(voxelGrid_13[v6FlatArrayId]),
                unpackVoxel(voxelGrid_13[v7FlatArrayId])
            );
            break;
    }

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

    // 8 * 8 = 64
    uint variantId = vertexOffsetZ * 64 + vertexOffsetY * 8 + vertexOffsetX;
    vec3 chunkPosition = chunkPositions[chunkBatchId].xyz;

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

    uint v0Replacer = (cube[replaceMap[v0Id].x].id == 255u) ? cube[replaceMap[v0Id].y].id : cube[replaceMap[v0Id].x].id;
    uint v1Replacer = (cube[replaceMap[v1Id].x].id == 255u) ? cube[replaceMap[v1Id].y].id : cube[replaceMap[v1Id].x].id;
    uint v2Replacer = (cube[replaceMap[v2Id].x].id == 255u) ? cube[replaceMap[v2Id].y].id : cube[replaceMap[v2Id].x].id;
    uint allInvalidId = (cube[v0Id].id == 255u) && (cube[v1Id].id == 255u) && (cube[v2Id].id == 255u) ? 1 : 0;
    uint allInvalidIdInv = (allInvalidId + 1) & 1;
    cube[v0Id].id = cube[v0Id].id * allInvalidIdInv + v0Replacer * allInvalidId;
    cube[v1Id].id = cube[v1Id].id * allInvalidIdInv + v1Replacer * allInvalidId;
    cube[v2Id].id = cube[v2Id].id * allInvalidIdInv + v2Replacer * allInvalidId;

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
