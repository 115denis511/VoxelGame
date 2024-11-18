#version 460 core

out vec2 texCoord;
out vec3 vertexTextureWeights;
out mat3 TBN;
flat out uint marchingCubeTextureIds[6];
flat out uvec3 triangleVertexVoxelIds;

layout(std140, binding = 0) uniform DrawVars
{
    mat4 projectionView;
    mat4 projection;
    mat4 view;
};

layout(std430, binding = 2) readonly buffer ChunkData 
{
    uvec2 packedData[];
};

struct VertexData {
    vec4 positions[8];
    uint vertexVoxelId;
    uint triangleId;
};
layout(std430, binding = 3) readonly buffer Verteces
{
    VertexData vertexData[];
};

struct UnpackedData {
    uint x;
    uint y;
    uint z;
    uint offsets[6];
    uint textureIds[6];
};

uniform vec3 chunkPosition;
const vec3 triangleVertexTextureWeights[3] = { vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)};
const int triangleShiftA[3] = { 0, -1, -2 };
const int triangleShiftB[3] = { 1,  0, -1 };
const int triangleShiftC[3] = { 2,  1,  0 };

UnpackedData unpackData(uvec2 data);

void main()
{
    int currentInstance = gl_BaseInstance + gl_InstanceID;
    UnpackedData data = unpackData(packedData[currentInstance]);
    int vertexTriangleId = gl_VertexID % 3;

    // POSITIONS
    vec3 triangleVertices[3];

    uint vertexId = gl_VertexID + triangleShiftA[vertexTriangleId];
    uint offsetStrength = data.offsets[vertexData[vertexId].vertexVoxelId];
    triangleVertices[0] = vertexData[vertexId].positions[offsetStrength].xyz;

    vertexId = gl_VertexID + triangleShiftB[vertexTriangleId];
    offsetStrength = data.offsets[vertexData[vertexId].vertexVoxelId];
    triangleVertices[1] = vertexData[vertexId].positions[offsetStrength].xyz;

    vertexId = gl_VertexID + triangleShiftC[vertexTriangleId];
    offsetStrength = data.offsets[vertexData[vertexId].vertexVoxelId];
    triangleVertices[2] = vertexData[vertexId].positions[offsetStrength].xyz;

    vec4 localPos = vec4(triangleVertices[vertexTriangleId].x + chunkPosition.x + data.x, 
                         triangleVertices[vertexTriangleId].y + chunkPosition.y + data.y, 
                         triangleVertices[vertexTriangleId].z + chunkPosition.z + data.z, 1.0);

    gl_Position = projectionView * localPos;

    // NORMAL
    vec3 edge_A = triangleVertices[1] - triangleVertices[0];
    vec3 edge_B = triangleVertices[2] - triangleVertices[0];
    vec3 normal = normalize(cross(edge_A, edge_B));

    // TEXTURE COORDS
    // get from https://stackoverflow.com/questions/8705201/troubles-with-marching-cubes-and-texture-coordinates
    vec3 normAbs = vec3(abs(normal.x), abs(normal.y), abs(normal.z));
    vec2 uvs[3];
    if (normAbs.x >= normAbs.z && normAbs.x >= normAbs.y) { // x plane
        uvs[0] = vec2(triangleVertices[0].z, triangleVertices[0].y);
        uvs[1] = vec2(triangleVertices[1].z, triangleVertices[1].y);
        uvs[2] = vec2(triangleVertices[2].z, triangleVertices[2].y);
    }
    else if (normAbs.z >= normAbs.x && normAbs.z >= normAbs.y) { // z plane
        uvs[0] = vec2(triangleVertices[0].x, triangleVertices[0].y);
        uvs[1] = vec2(triangleVertices[1].x, triangleVertices[1].y);
        uvs[2] = vec2(triangleVertices[2].x, triangleVertices[2].y);
    }
    else if (normAbs.y >= normAbs.x && normAbs.y >= normAbs.z) { // y plane
        uvs[0] = vec2(triangleVertices[0].x, triangleVertices[0].z);
        uvs[1] = vec2(triangleVertices[1].x, triangleVertices[1].z);
        uvs[2] = vec2(triangleVertices[2].x, triangleVertices[2].z);
    }

    texCoord = uvs[vertexTriangleId];
    vertexTextureWeights = triangleVertexTextureWeights[vertexTriangleId];
    marchingCubeTextureIds = data.textureIds;

    triangleVertexVoxelIds.x = vertexData[gl_VertexID - 2].vertexVoxelId;
    triangleVertexVoxelIds.y = vertexData[gl_VertexID - 1].vertexVoxelId;
    triangleVertexVoxelIds.z = vertexData[gl_VertexID].vertexVoxelId;

    // TANGENT, BITANGENT, TBN
    vec2 deltaUV_A = vec2(uvs[1].x - uvs[0].x, uvs[1].y - uvs[0].y);
    vec2 deltaUV_B = vec2(uvs[2].x - uvs[0].x, uvs[2].y - uvs[0].y);
    float f = 1.0f / (deltaUV_A.x * deltaUV_B.y - deltaUV_B.x * deltaUV_A.y);

    vec3 tangent = vec3(
        f * (deltaUV_B.y * edge_A.x - deltaUV_A.y * edge_B.x),
        f * (deltaUV_B.y * edge_A.y - deltaUV_A.y * edge_B.y),
        f * (deltaUV_B.y * edge_A.z - deltaUV_A.y * edge_B.z)  
    );
    tangent = normalize(tangent);

    vec3 bitangent = cross(normal, tangent);

    TBN = mat3(tangent,
               bitangent,
               normal
    ); 
}

UnpackedData unpackData(uvec2 data)
{
    UnpackedData unpaked;

    unpaked.x = (data[0] >> 25) & 31; // 31 = 0b11111
    unpaked.y = (data[0] >> 20) & 31; // 31 = 0b11111
    unpaked.z = (data[0] >> 15) & 31; // 31 = 0b11111
    unpaked.offsets[0] = (data[0] >> 12) & 7; // 7 = 0b111;
    unpaked.offsets[1] = (data[0] >> 9) & 7; // 7 = 0b111;
    unpaked.offsets[2] = (data[0] >> 6) & 7; // 7 = 0b111;
    unpaked.offsets[3] = (data[0] >> 3) & 7; // 7 = 0b111;
    unpaked.offsets[4] = data[0] & 7; // 7 = 0b111;

    unpaked.offsets[5] = (data.y >> 28) & 7; // 7 = 0b111;
    unpaked.textureIds[0] = (data.y >> 21) & 127; // 127 = 0b1111111;
    unpaked.textureIds[1] = (data.y >> 14) & 127; // 127 = 0b1111111;
    unpaked.textureIds[2] = (data.y >> 7) & 127; // 127 = 0b1111111;
    unpaked.textureIds[3] = data.y & 127; // 127 = 0b1111111

    unpaked.textureIds[4] = unpaked.textureIds[2];
    unpaked.textureIds[5] = unpaked.textureIds[3];

    return unpaked;
}