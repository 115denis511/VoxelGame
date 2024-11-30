#version 460 core

out vec2 texCoord;
out vec3 vertexTextureWeights;
out mat3 TBN;
flat out uint marchingCubeTextureIds[6];
flat out uvec3 triangleVertexVoxelIds;

layout(std140, binding = 0) uniform DrawVars
{
    mat4 projectionView;
    mat4 projection;
    mat4 view;
};

layout(std430, binding = 2) readonly buffer ChunkData 
{
    uvec2 packedData[];
};

struct TriangleVariant {
    vec4 pos_TBN_tex[6];
};
struct TriangleData {
    TriangleVariant variants[512];
    uvec4 vertexVoxelIds;
};
layout(std430, binding = 3) readonly buffer Verteces
{
    TriangleData triangleData[];
};

uniform vec3 chunkPosition;

struct UnpackedData {
    uint x;
    uint y;
    uint z;
    uint offsets[6];
    uint textureIds[6];
};

const vec3 triangleVertexTextureWeights[3] = { vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)};

UnpackedData unpackData(uvec2 data);

void main()
{
    int currentInstance = gl_BaseInstance + gl_InstanceID;
    UnpackedData data = unpackData(packedData[currentInstance]);

    int localTriangleVertexId = gl_VertexID % 3;
    int globalTriangleBaseVertexId = (gl_VertexID - localTriangleVertexId);
    int globalTriangleId = (globalTriangleBaseVertexId - gl_BaseVertex) / 3;

    uint vertexOffsetX = data.offsets[triangleData[globalTriangleId].vertexVoxelIds[0]];
    uint vertexOffsetY = data.offsets[triangleData[globalTriangleId].vertexVoxelIds[1]];
    uint vertexOffsetZ = data.offsets[triangleData[globalTriangleId].vertexVoxelIds[2]];

    // 8 * 8 = 64
    uint variantId = vertexOffsetZ * 64 + vertexOffsetY * 8 + vertexOffsetX;

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
    marchingCubeTextureIds = data.textureIds;

    triangleVertexVoxelIds.x = triangleData[globalTriangleId].vertexVoxelIds[0];
    triangleVertexVoxelIds.y = triangleData[globalTriangleId].vertexVoxelIds[1];
    triangleVertexVoxelIds.z = triangleData[globalTriangleId].vertexVoxelIds[2];
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
