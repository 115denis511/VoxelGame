#version 450 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D colorTex;
uniform sampler2D alphaTex;

void main()
{
    vec4 RGB = texture(colorTex, texCoord);
    float A = texture(alphaTex, texCoord).r;

    fragColor = vec4(RGB.rgb, A);
}