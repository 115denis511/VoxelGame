#version 450 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D colorTex;
uniform sampler2D alphaTex;
uniform vec4 alphaMask;

void main()
{
    vec4 RGB = texture(colorTex, texCoord);
    vec4 aplhaRGBA = texture(alphaTex, texCoord);
    float A = aplhaRGBA.r * alphaMask.r + 
              aplhaRGBA.g * alphaMask.g + 
              aplhaRGBA.b * alphaMask.b + 
              aplhaRGBA.a * alphaMask.a;

    fragColor = vec4(RGB.rgb, A);
}