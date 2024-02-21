#ifndef __RENDER__INSTANCING_MANAGER_H__
#define __RENDER__INSTANCING_MANAGER_H__

#include "../stdafx.h"
#include "../engine_properties.h"

namespace engine {
    class InstancingManager {
    public:
        static bool init();
        static void freeResources();

        static void pushMatrices(const glm::mat4* buffer, GLsizei count);

    private:
        static GLuint g_instancingMatricesSSBO;
    };
}

#endif