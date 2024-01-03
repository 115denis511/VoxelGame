#version 450 core

#extension GL_ARB_bindless_texture : require

in vec2 texCoord;

out vec4 fragColor;

layout(bindless_sampler) uniform sampler2DArray bindless;

layout(std140, binding = 1) uniform TexturePack
{
    sampler2DArray colorSpec;
    uint colorSpecLayer;
    uint colorSpecCapacity;
};

float getCoord(uint capacity, uint layer)
{
	return max(0, min(float(capacity - 1), floor(float(layer) + 0.5)));
}

void main()
{
    //fragColor = texture(textureDiffuse, texCoord);
    //fragColor = texture(texture_array, vec3(texCoord, getCoord(64, 0)));
    
    //fragColor = texture(bindless, vec3(texCoord, getCoord(64, 0)));
    //fragColor = vec4(texture(colorSpec, vec3(texCoord, getCoord(colorSpecCapacity, colorSpecLayer))));

    vec4 color = vec4(texture(colorSpec, vec3(texCoord, getCoord(colorSpecCapacity, colorSpecLayer))));
    fragColor = vec4(color.a, 0.f, 0.f, 1.f);

    //fragColor = vec4(0.6f);
    //fragColor = color;
}