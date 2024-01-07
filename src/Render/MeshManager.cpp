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

void engine::MeshManager::freeResources() {
    for (auto data : g_meshes) {
        delete data.second.mesh;
    }
}
