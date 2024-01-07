#include "AssetManager.h"

std::unordered_map<std::string, engine::Model*> engine::AssetManager::g_models;

engine::Model *engine::AssetManager::addModel(const std::string &path) {
    if (g_models.contains(path)) {
        return g_models[path];
    }

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Log::addFatalError(std::string("ERROR! ASSIMP::") + import.GetErrorString());
        return nullptr;
    }

    Model* model = new Model(scene, path);

    g_models[path] = model;

    return model;
}

engine::Model *engine::AssetManager::getModel(const std::string &path) {
    if (g_models.contains(path)) {
        return g_models[path];
    }
    return nullptr;
}

bool engine::AssetManager::init(Shader* shaderMix_RGB_A, Shader* shaderFill_RGB) {
    if (!UniformManager::init()) {
        return false;
    }

    Mesh* fillRect = MeshManager::getPrimitiveRect(-1.f, 1.f, 1.f, -1.f);
    if (!TextureManager::init(shaderMix_RGB_A, shaderFill_RGB, fillRect)) {
        return false;
    }

    return true;
}

void engine::AssetManager::freeResources() {    
    UniformManager::freeResources();
    TextureManager::freeResources();
    MeshManager::freeResources();
}
