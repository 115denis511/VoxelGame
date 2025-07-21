#ifndef __RENDER__UNIFORM_MANAGER_H__
#define __RENDER__UNIFORM_MANAGER_H__

#include "../stdafx.h"
#include "../engine_properties.h"
#include "UniformBuffer.h"
#include "UniformStructs.h"

namespace engine {
    class AssetManager;

    class UniformManager {
        friend AssetManager;

    public:
        static void setDrawVars(uniform_structs::DrawVars& vars);
        static void setTexturePack(uniform_structs::TexturePack& vars);
        static void setChunkPositions(const std::array<glm::vec4, 14>& positions);

    private:
        UniformManager() {}

        static UniformBuffer* g_drawVars;
        static UniformBuffer* g_texturePack;
        static UniformBuffer* g_chunkPositions;

        static bool init();
        static void freeResources();
    };
}

#endif