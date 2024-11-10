#ifndef __RENDER__ASSET_MANAGER_H__
#define __RENDER__ASSET_MANAGER_H__

#include "../stdafx.h"
#include "../Log.h"
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
        static Model* addPrimitiveRectangle(GLfloat leftX, GLfloat topY, GLfloat rightX, GLfloat bottomY, const TextureArrayRef &colorSpecTexture, std::string name = "");
        static Model* addPrimitiveCube(GLfloat size, const TextureArrayRef &colorSpecTexture, std::string name = "");
        static Model* addPrimitiveSphere(float radius, int sectorCount, int stackCount, const TextureArrayRef &colorSpecTexture, std::string name = "");
        static Model* getModel(const std::string& path);

    private:
        static std::unordered_map<std::string, Model*> g_models;

        static bool init(Shader* shaderMix_RGB_A, Shader* shaderFill_RGB);
        static void freeResources();
    };
}

#endif