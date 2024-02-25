#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

layout(std140, binding = 0) uniform DrawVars
{
    mat4 projectionView;
};

layout(std430, binding = 0) readonly buffer InstansingBuffer 
{
    int matrixId[];
};
layout(std140, binding = 1) readonly buffer TransformsBuffer 
{
    mat4 modelMatrices[];
};

out vec2 texCoord;

uniform mat4 model;

void main()
{
    //gl_Position = projectionView * model * vec4(Position.xyz, 1.0);
    int transformId = matrixId[gl_InstanceID];
    gl_Position = projectionView * modelMatrices[transformId] * vec4(Position.xyz, 1.0);
    texCoord = TexCoord;
}