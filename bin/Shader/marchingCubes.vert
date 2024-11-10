#version 460 core

// Mapping of VertexPack:
// voxelSize - value from 0 to 7, stored in packedData's offset[6]
// position - VertexPack[voxelSize].xyz or xyz of first 8 positions of array
// normal   - VertexPack[voxelSize + 8].xyz or xyz of last 8 positions of array
// texCoord - VertexPack[voxelSize * 2].w for X and VertexPack[voxelSize * 2 + 1].w for Y
//            Its means texCoors stored in last COLUMN of array
layout (location = 0) in vec4 VertexPack[16];

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
    vec4 tangents[8];
    vec4 bitangents[8];
    uint vertexVoxelId;
};
layout(std430, binding = 3) readonly buffer Verteces
{
    VertexData vertexData[];
};

struct UnpackedData {
    uint x;
    uint y;
    uint z;
    uint offsets[6];
    uint textureIds[6];
};

uniform vec3 chunkPosition;
const vec3 triangleVertexTextureWeights[3] = { vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)};

UnpackedData unpackData(uvec2 data);

void main()
{
    int currentInstance = gl_BaseInstance + gl_InstanceID;
    UnpackedData data = unpackData(packedData[currentInstance]);
    int vertexTriangleId = gl_VertexID % 3;
    uint offsetId = data.offsets[vertexData[gl_VertexID].vertexVoxelId];

    vec3 cornerPos = vec3(VertexPack[offsetId].x + chunkPosition.x, 
                          VertexPack[offsetId].y + chunkPosition.y, 
                          VertexPack[offsetId].z + chunkPosition.z);
    vec4 localPos = vec4(cornerPos.x + data.x, cornerPos.y + data.y, cornerPos.z + data.z, 1.0);

    gl_Position = projectionView * localPos;

    triangleVertexVoxelIds.x = vertexData[gl_VertexID - 2].vertexVoxelId;
    triangleVertexVoxelIds.y = vertexData[gl_VertexID - 1].vertexVoxelId;
    triangleVertexVoxelIds.z = vertexData[gl_VertexID].vertexVoxelId;

    // TECTURE COORD
    uint texId = offsetId * 2;
    texCoord = vec2(VertexPack[texId].w, VertexPack[texId + 1].w);
    vertexTextureWeights = triangleVertexTextureWeights[vertexTriangleId];
    
    marchingCubeTextureIds = data.textureIds;

    // TBN
    TBN = mat3(vertexData[gl_VertexID].tangents[offsetId].xyz,      // Tangent
               vertexData[gl_VertexID].bitangents[offsetId].xyz,    // Bitangent
               VertexPack[offsetId + 8].xyz                         // Normal
    ); 
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