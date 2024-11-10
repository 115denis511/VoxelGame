#include "MeshManager.h"

std::unordered_map<std::string, engine::MeshRelatedData> engine::MeshManager::g_meshes;

engine::Mesh* engine::MeshManager::addMesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const uniform_structs::TexturePack &textures, const std::string& name) {
    if (g_meshes.contains(name)) {
        Log::addMessage("engine::MeshManager::addMesh(const Mesh *mesh, const uniform_structs::TexturePack &textures, std::string_view name): "
                        "WARNING! Mesh with this name already exist\nName: " + name);
        return g_meshes[name].mesh;
    }

    MeshRelatedData data = {
        new Mesh(vertices, indices),
        textures
    };
    g_meshes[name] = data;

    return data.mesh;
}

engine::MeshRelatedData engine::MeshManager::getMesh(std::string &name) {
    if (!g_meshes.contains(name)) {
        return MeshRelatedData();
    }

    return g_meshes[name];
}

engine::Mesh *engine::MeshManager::getPrimitiveRect(GLfloat leftX, GLfloat topY, GLfloat rightX, GLfloat bottomY) {
    std::string name = + "__primitiveRect_" + std::to_string(leftX) + "_" + std::to_string(topY) + "_" + std::to_string(rightX) + "_" + std::to_string(bottomY) + "__";

    if (g_meshes.contains(name)) {
        return g_meshes[name].mesh;
    }

    Vertex vertices[] = {
        Vertex{glm::vec3(leftX,  bottomY, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(rightX, bottomY, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(rightX, topY,    0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(leftX,  topY,    0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)}
    };

    constexpr GLuint indeces[] = {
        0, 1, 2,   // Первый треугольник
        0, 2, 3,   // Второй треугольник
    };

    Mesh* mesh = new Mesh(vertices, std::size(vertices), indeces, std::size(indeces));
    MeshRelatedData data = {
        mesh
    };
    g_meshes[name] = data;

    return mesh;
}

engine::Mesh *engine::MeshManager::getPrimitiveCube(GLfloat size) {
    std::string name = "__primitiveCube_" + std::to_string(size) + "__";

    if (g_meshes.contains(name)) {
        return g_meshes[name].mesh;
    }

    Vertex vertices[] = {
        Vertex{glm::vec3(-size, -size, -size), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3( size, -size, -size), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3( size,  size, -size), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(-size,  size, -size), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},

        Vertex{glm::vec3(-size, -size, size), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3( size, -size, size), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3( size, size,  size), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(-size, size,  size), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},

        Vertex{glm::vec3(-size,  size,  size), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(-size,  size, -size), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(-size, -size, -size), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(-size, -size,  size), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},

        Vertex{glm::vec3(size,  size,  size), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(size,  size, -size), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(size, -size, -size), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(size, -size,  size), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},

        Vertex{glm::vec3(-size, -size, -size), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3( size, -size, -size), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3( size, -size,  size), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(-size, -size,  size), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},

        Vertex{glm::vec3(-size, size, -size), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3( size, size, -size), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3( size, size,  size), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(-size, size,  size), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)}
    };

    constexpr GLuint indeces[] = {
        // Back
        3, 1, 0,   // Первый треугольник
        3, 2, 1,   // Второй треугольник
        // Front
        7, 4, 5,
        7, 5, 6,
        // Left
        8, 9, 10,
        8, 10, 11,
        // Right
        12, 14, 13,
        12, 15, 14,
        // Bottom
        16, 17, 18,
        19, 16, 18,
        // Top
        22, 21, 20,
        22, 20, 23
    };

    Mesh* mesh = new Mesh(vertices, std::size(vertices), indeces, std::size(indeces));
    MeshRelatedData data = {
        mesh
    };
    g_meshes[name] = data;

    return mesh;
}

engine::Mesh *engine::MeshManager::getPrimitiveSphere(float radius, int sectorCount, int stackCount) {
    std::string name = "__primitiveSphere_" + std::to_string(radius) + "_" + std::to_string(sectorCount) + "_" + std::to_string(stackCount) + "__";

    if (g_meshes.contains(name)) {
        return g_meshes[name].mesh;
    }

    //https://songho.ca/opengl/gl_sphere.html
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    float x, y, z, xz;                      // vertex position
    float lengthInv = 1.0f / radius;        // vertex normal length
    constexpr double PI = M_PI;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i){
        Vertex vertex;

        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xz = radius * std::cos(stackAngle);             // r * cos(u)
        y = radius * std::sin(stackAngle);              // r * sin(u)

        for(int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi
            
            x = xz * std::sin(sectorAngle);             // r * cos(u) * cos(v)
            z = xz * std::cos(sectorAngle);             // r * cos(u) * sin(v)
            vertex.position.x = x;
            vertex.position.y = y;
            vertex.position.z = z;

            vertex.normal.x = x * lengthInv;
            vertex.normal.y = y * lengthInv;
            vertex.normal.z = z * lengthInv;

            vertex.texCoord.s = (float)j / sectorCount;
            vertex.texCoord.t = (float)i / stackCount;

            vertex.boneIds.x = -1;
            vertex.boneIds.y = -1;
            vertex.boneIds.z = -1;
            vertex.boneIds.w = -1;
            vertex.boneWeights.x = 0.f;
            vertex.boneWeights.y = 0.f;
            vertex.boneWeights.z = 0.f;
            vertex.boneWeights.w = 0.f;

            vertices.push_back(vertex);
        }
    }

    int k1, k2;
    for(int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    Mesh* mesh = new Mesh(vertices, indices);
    MeshRelatedData data = {
        mesh
    };
    g_meshes[name] = data;

    return mesh;
}

std::string engine::MeshManager::getRectName(GLfloat leftX, GLfloat topY, GLfloat rightX, GLfloat bottomY) {
    return "__primitiveRect_" + std::to_string(leftX) + "_" + std::to_string(topY) + "_" + std::to_string(rightX) + "_" + std::to_string(bottomY) + "__";
}

std::string engine::MeshManager::getCubeName(GLfloat size) {
    return "__primitiveCube_" + std::to_string(size) + "__";
}

std::string engine::MeshManager::getSphereName(float radius, int sectorCount, int stackCount) {
    return "__primitiveSphere_" + std::to_string(radius) + "_" + std::to_string(sectorCount) + "_" + std::to_string(stackCount) + "__";
}

void engine::MeshManager::freeResources() {
    for (auto data : g_meshes) {
        delete data.second.mesh;
    }
}
