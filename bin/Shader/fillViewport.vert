#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(Position.xyz, 1.0);
    texCoord = TexCoord;
}