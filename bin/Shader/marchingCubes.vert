#version 460 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in int VoxelId;
layout (location = 4) in int OffsetDirection;

out vec2 texCoord;

layout(std140, binding = 0) uniform DrawVars
{
    mat4 projectionView;
    mat4 projection;
    mat4 view;
};

// !!!binding id is for debug purpose, change later!!!
layout(std430, binding = 2) readonly buffer ChunkData 
{
    ivec2 packedData[];
};

struct UnpackedData {
    int x;
    int y;
    int z;
    int offsets[6];
    int textureIds[6];
};

uniform vec3 chunkPosition;

UnpackedData unpackData(ivec2 data);

void main()
{
    int currentInstance = gl_BaseInstance + gl_InstanceID;
    UnpackedData data = unpackData(packedData[currentInstance]);
    vec3 cornerPos = vec3(Position.x + 0.5 + chunkPosition.x, Position.y + 0.5 + chunkPosition.y, Position.z + 0.5 + chunkPosition.z);
    vec4 localPos = vec4(cornerPos.x + data.x, cornerPos.y + data.y, cornerPos.z + data.z, 1.0);
    //vec4 localPos = vec4(Position.x + gl_InstanceID, Position.y, Position.z, 1.0);
    gl_Position = projectionView * localPos;//vec4(Position.xyz, 1.0);
    //gl_Position = projection * view * vec4(Position.xyz, 1.0);

    texCoord = TexCoord;
}

UnpackedData unpackData(ivec2 data)
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

    unpaked.offsets[5] = (data[1] >> 28) & 7; // 7 = 0b111;
    unpaked.textureIds[0] = (data[1] >> 21) & 127; // 127 = 0b1111111;
    unpaked.textureIds[1] = (data[1] >> 14) & 127; // 127 = 0b1111111;
    unpaked.textureIds[2] = (data[1] >> 7) & 127; // 127 = 0b1111111;
    unpaked.textureIds[3] = data[1] & 127; // 127 = 0b1111111;
    unpaked.textureIds[4] = unpaked.textureIds[2];
    unpaked.textureIds[5] = unpaked.textureIds[3];

    return unpaked;
}