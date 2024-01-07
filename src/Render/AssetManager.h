#ifndef __RENDER__ASSET_MANAGER_H__
#define __RENDER__ASSET_MANAGER_H__

#include "../stdafx.h"
#include "UniformManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "Model.h"
#include "Shader.h"

namespace engine {
    class Render;

    class AssetManager {
        friend Render;

    public:
        static Model* addModel(const std::string& path);
        static Model* getModel(const std::string& path);

    private:
        static std::unordered_map<std::string, Model*> g_models;

        static bool init(Shader* shaderMix_RGB_A, Shader* shaderFill_RGB);
        static void freeResources();
    };
}

#endif