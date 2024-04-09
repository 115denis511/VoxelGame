#version 460 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D tex;

void main()
{
    //fragColor = vec4(1.f);
    fragColor = texture(tex, texCoord);
}