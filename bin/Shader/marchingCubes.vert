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
layout(std430, binding = 14) readonly buffer Verteces // src/Voxel/MarchingCubesManager.h::SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS
{
    TriangleData triangleData[];
};

layout(std430, binding = 15) readonly buffer ChunkRefs // src/Voxel/MarchingCubesManager.h::SSBO_BLOCK__DRAW_ID_TO_CHUNK
{
    int chunkRefs[];
};

struct UnpackedData {
    uint x;
    uint y;
    uint z;
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

UnpackedData unpackData(uint data);
UnpackedVoxel unpackVoxel(uint raw);

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
    uint v0FlatArrayId, v1FlatArrayId, v2FlatArrayId;
    UnpackedVoxel v0, v1, v2;
    switch (chunkBatchId) {
        case 0:
            data = unpackData(packedData_0[currentInstance]);

            v0FlatArrayId = ((data.z + idOffset[v0Id].z) * 1089) + ((data.y + idOffset[v0Id].y) * 33) + (data.x + idOffset[v0Id].x);
            v1FlatArrayId = ((data.z + idOffset[v1Id].z) * 1089) + ((data.y + idOffset[v1Id].y) * 33) + (data.x + idOffset[v1Id].x);
            v2FlatArrayId = ((data.z + idOffset[v2Id].z) * 1089) + ((data.y + idOffset[v2Id].y) * 33) + (data.x + idOffset[v2Id].x);

            v0 = unpackVoxel(voxelGrid_1[v0FlatArrayId]);
            v1 = unpackVoxel(voxelGrid_1[v1FlatArrayId]);
            v2 = unpackVoxel(voxelGrid_1[v2FlatArrayId]);
            break;
        case 1:
            data = unpackData(packedData_2[currentInstance]);
            
            v0FlatArrayId = ((data.z + idOffset[v0Id].z) * 1089) + ((data.y + idOffset[v0Id].y) * 33) + (data.x + idOffset[v0Id].x);
            v1FlatArrayId = ((data.z + idOffset[v1Id].z) * 1089) + ((data.y + idOffset[v1Id].y) * 33) + (data.x + idOffset[v1Id].x);
            v2FlatArrayId = ((data.z + idOffset[v2Id].z) * 1089) + ((data.y + idOffset[v2Id].y) * 33) + (data.x + idOffset[v2Id].x);

            v0 = unpackVoxel(voxelGrid_3[v0FlatArrayId]);
            v1 = unpackVoxel(voxelGrid_3[v1FlatArrayId]);
            v2 = unpackVoxel(voxelGrid_3[v2FlatArrayId]);
            break;
        case 2:
            data = unpackData(packedData_4[currentInstance]);
            
            v0FlatArrayId = ((data.z + idOffset[v0Id].z) * 1089) + ((data.y + idOffset[v0Id].y) * 33) + (data.x + idOffset[v0Id].x);
            v1FlatArrayId = ((data.z + idOffset[v1Id].z) * 1089) + ((data.y + idOffset[v1Id].y) * 33) + (data.x + idOffset[v1Id].x);
            v2FlatArrayId = ((data.z + idOffset[v2Id].z) * 1089) + ((data.y + idOffset[v2Id].y) * 33) + (data.x + idOffset[v2Id].x);

            v0 = unpackVoxel(voxelGrid_5[v0FlatArrayId]);
            v1 = unpackVoxel(voxelGrid_5[v1FlatArrayId]);
            v2 = unpackVoxel(voxelGrid_5[v2FlatArrayId]);
            break;
        case 3:
            data = unpackData(packedData_6[currentInstance]);
            
            v0FlatArrayId = ((data.z + idOffset[v0Id].z) * 1089) + ((data.y + idOffset[v0Id].y) * 33) + (data.x + idOffset[v0Id].x);
            v1FlatArrayId = ((data.z + idOffset[v1Id].z) * 1089) + ((data.y + idOffset[v1Id].y) * 33) + (data.x + idOffset[v1Id].x);
            v2FlatArrayId = ((data.z + idOffset[v2Id].z) * 1089) + ((data.y + idOffset[v2Id].y) * 33) + (data.x + idOffset[v2Id].x);

            v0 = unpackVoxel(voxelGrid_7[v0FlatArrayId]);
            v1 = unpackVoxel(voxelGrid_7[v1FlatArrayId]);
            v2 = unpackVoxel(voxelGrid_7[v2FlatArrayId]);
            break;
        case 4:
            data = unpackData(packedData_8[currentInstance]);
            
            v0FlatArrayId = ((data.z + idOffset[v0Id].z) * 1089) + ((data.y + idOffset[v0Id].y) * 33) + (data.x + idOffset[v0Id].x);
            v1FlatArrayId = ((data.z + idOffset[v1Id].z) * 1089) + ((data.y + idOffset[v1Id].y) * 33) + (data.x + idOffset[v1Id].x);
            v2FlatArrayId = ((data.z + idOffset[v2Id].z) * 1089) + ((data.y + idOffset[v2Id].y) * 33) + (data.x + idOffset[v2Id].x);

            v0 = unpackVoxel(voxelGrid_9[v0FlatArrayId]);
            v1 = unpackVoxel(voxelGrid_9[v1FlatArrayId]);
            v2 = unpackVoxel(voxelGrid_9[v2FlatArrayId]);
            break;
        case 5:
            data = unpackData(packedData_10[currentInstance]);
            
            v0FlatArrayId = ((data.z + idOffset[v0Id].z) * 1089) + ((data.y + idOffset[v0Id].y) * 33) + (data.x + idOffset[v0Id].x);
            v1FlatArrayId = ((data.z + idOffset[v1Id].z) * 1089) + ((data.y + idOffset[v1Id].y) * 33) + (data.x + idOffset[v1Id].x);
            v2FlatArrayId = ((data.z + idOffset[v2Id].z) * 1089) + ((data.y + idOffset[v2Id].y) * 33) + (data.x + idOffset[v2Id].x);

            v0 = unpackVoxel(voxelGrid_11[v0FlatArrayId]);
            v1 = unpackVoxel(voxelGrid_11[v1FlatArrayId]);
            v2 = unpackVoxel(voxelGrid_11[v2FlatArrayId]);
            break;
        case 6:
            data = unpackData(packedData_12[currentInstance]);
            
            v0FlatArrayId = ((data.z + idOffset[v0Id].z) * 1089) + ((data.y + idOffset[v0Id].y) * 33) + (data.x + idOffset[v0Id].x);
            v1FlatArrayId = ((data.z + idOffset[v1Id].z) * 1089) + ((data.y + idOffset[v1Id].y) * 33) + (data.x + idOffset[v1Id].x);
            v2FlatArrayId = ((data.z + idOffset[v2Id].z) * 1089) + ((data.y + idOffset[v2Id].y) * 33) + (data.x + idOffset[v2Id].x);

            v0 = unpackVoxel(voxelGrid_13[v0FlatArrayId]);
            v1 = unpackVoxel(voxelGrid_13[v1FlatArrayId]);
            v2 = unpackVoxel(voxelGrid_13[v2FlatArrayId]);
            break;
    }

    uint vertexOffsetX = v0.size;
    uint vertexOffsetY = v1.size;
    uint vertexOffsetZ = v2.size;

    // 8 * 8 = 64
    uint variantId = vertexOffsetZ * 64 + vertexOffsetY * 8 + vertexOffsetX;
    vec3 chunkPosition = chunkPositions[chunkBatchId].xyz;

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

UnpackedData unpackData(uint data)
{
    UnpackedData unpaked;

    unpaked.x = (data >> 10) & 31; // 31 = 0b11111
    unpaked.y = (data >> 5) & 31; // 31 = 0b11111
    unpaked.z = (data) & 31; // 31 = 0b11111

    return unpaked;
}

UnpackedVoxel unpackVoxel(uint raw)
{
    UnpackedVoxel unpacked;

    unpacked.id = raw & 255; // 255 = 0bFF
    unpacked.size = (raw >> 8) & 7; // 7 = 0b111

    return unpacked;
}
