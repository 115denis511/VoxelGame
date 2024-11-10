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

engine::Model *engine::AssetManager::addPrimitiveRectangle(GLfloat leftX, GLfloat topY, GLfloat rightX, GLfloat bottomY, 
                                                           const TextureArrayRef &colorSpecTexture, std::string name) {
    if (name.empty())
        name = MeshManager::getRectName(leftX, topY, rightX, bottomY) + "_" + std::to_string(colorSpecTexture.getHandler()) + "__";

    if (g_models.contains(name)) {
        //Log::addMessage("engine::AssetManager::addPrimitiveRectangle WARNING! Mesh with this name already exist\nName: " + name);
        return g_models[name];
    }

    Mesh* mesh = MeshManager::getPrimitiveRect(leftX, topY, rightX, bottomY);
    Model* model = new Model(*mesh, colorSpecTexture);
    float maxSize = std::max(std::max(std::abs(leftX), std::abs(rightX)), std::max(std::abs(topY), std::abs(bottomY)));
    model->m_cullingVolume = SphereVolume(glm::vec3(0.f), std::sqrt((maxSize * maxSize) * 2));
    g_models[name] = model;

    return model;
}

engine::Model *engine::AssetManager::addPrimitiveCube(GLfloat size, const TextureArrayRef &colorSpecTexture, std::string name) {
    if (name.empty())
        name = MeshManager::getCubeName(size) + "_" + std::to_string(colorSpecTexture.getHandler()) + "__";

    if (g_models.contains(name)) {
        //Log::addMessage("engine::AssetManager::addPrimitiveCube WARNING! Mesh with this name already exist\nName: " + name);
        return g_models[name];
    }

    Mesh* mesh = MeshManager::getPrimitiveCube(size);
    Model* model = new Model(*mesh, colorSpecTexture);
    model->m_cullingVolume = SphereVolume(glm::vec3(0.f), std::sqrt((size * size) * 2));
    g_models[name] = model;

    return model;
}

engine::Model *engine::AssetManager::addPrimitiveSphere(float radius, int sectorCount, int stackCount, 
                                                        const TextureArrayRef &colorSpecTexture, std::string name) {
    if (name.empty())
        name = MeshManager::getSphereName(radius, sectorCount, stackCount) + "_" + std::to_string(colorSpecTexture.getHandler()) + "__";

    if (g_models.contains(name)) {
        //Log::addMessage("engine::AssetManager::addPrimitiveSphere WARNING! Mesh with this name already exist\nName: " + name);
        return g_models[name];
    }

    Mesh* mesh = MeshManager::getPrimitiveSphere(radius, sectorCount, stackCount);
    Model* model = new Model(*mesh, colorSpecTexture);
    model->m_cullingVolume = SphereVolume(glm::vec3(0.f), radius);
    g_models[name] = model;

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
