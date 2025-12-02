#version 460 core

#extension GL_ARB_bindless_texture : require

in vec2 texCoord;
in vec3 vertexTextureWeights;
in mat3 TBN;
flat in uvec3 triangleVertexIds;

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
    uint layer0 = triangleVertexIds.x;
    uint layer1 = triangleVertexIds.y;
    uint layer2 = triangleVertexIds.z;

    vec4 tex0 = vec4(texture(texArray, vec3(texCoord, getCoord(capacity, layer0))));
    vec4 tex1 = vec4(texture(texArray, vec3(texCoord, getCoord(capacity, layer1))));
    vec4 tex2 = vec4(texture(texArray, vec3(texCoord, getCoord(capacity, layer2))));

    // https://outpostengineer.com/barycentricShader.html
    float b = 0.3;
    vec3 weights = vec3(max(0, vertexTextureWeights.r - b), max(0, vertexTextureWeights.g - b), max(0, vertexTextureWeights.b - b));
    float sum = weights.r + weights.g + weights.b;
    weights = vec3(weights.r / sum, weights.g / sum, weights.b / sum);

    fragColor = vec4(
        tex0.x * weights.x + tex1.x * weights.y + tex2.x * weights.z,
        tex0.y * weights.x + tex1.y * weights.y + tex2.y * weights.z,
        tex0.z * weights.x + tex1.z * weights.y + tex2.z * weights.z,
        1.0
    );

    // TBN test
    /*vec3 normalTest = vec3(0.0, 0.0, 1.0);
    //normalTest = normalize(normalTest * 2.0 - 1.0); // this is needed for normal from textures
    normalTest = normalize(TBN * normalTest);
    fragColor = vec4(normalTest.xyz, 1.0);*/ 
}