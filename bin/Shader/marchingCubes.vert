#version 460 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in int VoxelId;
layout (location = 4) in int OffsetDirection;

out vec2 texCoord;
out vec3 vertexTextureWeights;
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
layout(std430, binding = 3) readonly buffer VertexVoxelIds
{
    uint vertexVoxelIds[];
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
// X_LEFT = 0, X_RIGHT = 1, Y_UP = 2, Y_DOWN = 3, Z_FRONT = 4, Z_BACK = 5;
const vec3 directionMul[6] = { vec3(-1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, -1.0) };
const float offsetsStrengh[8] = { -0.375, -0.25, -0.125, 0.0, 0.125, 0.25, 0.375, 0.5 };

UnpackedData unpackData(uvec2 data);

void main()
{
    int currentInstance = gl_BaseInstance + gl_InstanceID;
    UnpackedData data = unpackData(packedData[currentInstance]);
    vec3 cornerPos = vec3(Position.x + 0.5 + chunkPosition.x, Position.y + 0.5 + chunkPosition.y, Position.z + 0.5 + chunkPosition.z);
    vec4 localPos = vec4(cornerPos.x + data.x, cornerPos.y + data.y, cornerPos.z + data.z, 1.0);

    uint offsetId = data.offsets[vertexVoxelIds[gl_VertexID]];
    float offsetStrengh = offsetsStrengh[offsetId];
    vec3 offsetDirMul = directionMul[OffsetDirection];
    localPos = vec4(localPos.x + (offsetStrengh * offsetDirMul.x), localPos.y + (offsetStrengh * offsetDirMul.y), localPos.z + (offsetStrengh * offsetDirMul.z), 1.0);

    gl_Position = projectionView * localPos;

    triangleVertexVoxelIds.x = vertexVoxelIds[gl_VertexID - 2];
    triangleVertexVoxelIds.y = vertexVoxelIds[gl_VertexID - 1];
    triangleVertexVoxelIds.z = vertexVoxelIds[gl_VertexID];

    texCoord = TexCoord;
    vertexTextureWeights = triangleVertexTextureWeights[gl_VertexID % 3];
    
    marchingCubeTextureIds = data.textureIds;
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