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
struct VertexData {
    vec4 positions[8];
    uint vertexVoxelId;
    uint triangleId;
};
layout(std430, binding = 3) readonly buffer Verteces
{
    VertexData vertexData[];
};
struct TriangleData {
    vec4 pos_TBN_tex[6];
    uvec4 figureIds;
};
layout(std430, binding = 5) buffer OutputBuffer  
{
    TriangleData triangleData[]; // 297910
};

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
    uint currentInstance = gl_BaseInstance + gl_InstanceID;
    UnpackedData data = unpackData(packedData[currentInstance]);
    int vertexTriangleId = gl_VertexID % 3;
    uint triangleDataId = currentInstance * 5 + vertexData[gl_VertexID].triangleId;

    gl_Position = projectionView * vec4(triangleData[triangleDataId].pos_TBN_tex[vertexTriangleId].xyz, 1.0);

    TBN = mat3(triangleData[triangleDataId].pos_TBN_tex[3].xyz,
               triangleData[triangleDataId].pos_TBN_tex[4].xyz,
               triangleData[triangleDataId].pos_TBN_tex[5].xyz
    ); 

    uint texCoordLocation = vertexTriangleId * 2;
    texCoord = vec2(
        triangleData[triangleDataId].pos_TBN_tex[texCoordLocation].w, 
        triangleData[triangleDataId].pos_TBN_tex[texCoordLocation + 1].w
    );
    vertexTextureWeights = triangleVertexTextureWeights[vertexTriangleId];
    marchingCubeTextureIds = data.textureIds;

    triangleVertexVoxelIds.x = vertexData[gl_VertexID - 2].vertexVoxelId;
    triangleVertexVoxelIds.y = vertexData[gl_VertexID - 1].vertexVoxelId;
    triangleVertexVoxelIds.z = vertexData[gl_VertexID].vertexVoxelId;
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