#version 450 core

in vec2 texCoord;

out vec4 fragColor;
//layout (location = 0) out vec4 fragColor;
//layout (location = 1) out vec4 brightColor;

layout (std140) uniform DrawVars
{
    mat4 projectionView;
};

uniform sampler2D textureDiffuse;

void main()
{
    //fragColor = texture(textureDiffuse, texCoord);
    fragColor = vec4(1.f);
    //fragColor = color;
}