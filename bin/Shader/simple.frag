#version 450 core

#extension GL_ARB_bindless_texture : require

in vec2 texCoord;

out vec4 fragColor;

//uniform sampler2D textureDiffuse;
//uniform sampler2DArray texture_array;
//uniform uvec2 texture_array;
//uniform uvec2 bindless;
layout(bindless_sampler) uniform sampler2DArray bindless;

float getCoord(uint capacity, uint layer)
{
	return max(0, min(float(capacity - 1), floor(float(layer) + 0.5)));
}

void main()
{
    //fragColor = texture(textureDiffuse, texCoord);
    //fragColor = texture(texture_array, vec3(texCoord, getCoord(64, 0)));
    
    fragColor = texture(bindless, vec3(texCoord, getCoord(64, 0)));

    //fragColor = vec4(0.6f);
    //fragColor = color;
}