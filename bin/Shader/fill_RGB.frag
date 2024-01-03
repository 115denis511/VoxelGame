#version 450 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D colorTex;

void main()
{
    vec4 RGB = texture(colorTex, texCoord);
    float A = 0.f;

    fragColor = vec4(RGB.rgb, A);
}