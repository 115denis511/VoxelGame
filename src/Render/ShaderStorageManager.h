#ifndef __RENDER__SHADER_STORAGE_MANAGER_H__
#define __RENDER__SHADER_STORAGE_MANAGER_H__

#include "../stdafx.h"
#include "../engine_properties.h"

namespace engine {
    class ShaderStorageManager {
    public:
        static bool init();
        static void freeResources();

        static void pushInstancingTransformIds(const int* buffer, GLsizei count);
        static glm::mat4* getMappedTransformsSSBO();
        static void unmapTransformsSSBO();

    private:
        static GLuint g_instancingMatricesSSBO;
        static GLuint g_transformMatricesSSBO;
        static GLuint g_computeOutputSSBO;
    };
}

#endif