#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

out vec2 texCoord;

layout (std140) uniform MatricesMVP
{
    mat4 MVP[1024];
};
layout (std140) uniform MatricesModel
{
    mat4 modelMatrices[1024];
};
layout (std140) uniform NormalMatrices
{
    mat3 drawNormalMatrices[1024];
};
layout (std140) uniform DrawVars
{
    mat4 projectionView;
};

void main()
{
    gl_Position = MVP[gl_InstanceID] * vec4(Position.xyz, 1.0);
    texCoord = TexCoord;
}