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

void engine::MeshManager::freeResources() {
    for (auto data : g_meshes) {
        delete data.second.mesh;
    }
}
