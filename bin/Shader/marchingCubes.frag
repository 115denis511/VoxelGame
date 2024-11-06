#version 460 core

#extension GL_ARB_bindless_texture : require

in vec2 texCoord;
in vec3 vertexTextureWeights;
flat in uint marchingCubeTextureIds[6];
flat in uvec3 triangleVertexVoxelIds;

out vec4 fragColor;

layout(std140, binding = 1) uniform TexturePack
{
    sampler2DArray texArray;
    uint usedLayer_unused;
    uint capacity;
};

float getCoord(uint capacity, uint layer)
{
	return max(0, min(float(capacity - 1), floor(float(layer) + 0.5)));
}

void main()
{
    //fragColor = vec4(texture(texArray, vec3(texCoord, getCoord(capacity, triangleVertexVoxelIds.x))));
    uint layer0 = marchingCubeTextureIds[triangleVertexVoxelIds.x];
    uint layer1 = marchingCubeTextureIds[triangleVertexVoxelIds.y];
    uint layer2 = marchingCubeTextureIds[triangleVertexVoxelIds.z];

    vec4 tex0 = vec4(texture(texArray, vec3(texCoord, getCoord(capacity, layer0))));
    vec4 tex1 = vec4(texture(texArray, vec3(texCoord, getCoord(capacity, layer1))));
    vec4 tex2 = vec4(texture(texArray, vec3(texCoord, getCoord(capacity, layer2))));

    fragColor = vec4(
        tex0.x * vertexTextureWeights.x + tex1.x * vertexTextureWeights.y + tex2.x * vertexTextureWeights.z,
        tex0.y * vertexTextureWeights.x + tex1.y * vertexTextureWeights.y + tex2.y * vertexTextureWeights.z,
        tex0.z * vertexTextureWeights.x + tex1.z * vertexTextureWeights.y + tex2.z * vertexTextureWeights.z,
        1.0
    );
}