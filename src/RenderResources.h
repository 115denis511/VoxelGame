#ifndef __RENDER_RESOURCES_H__
#define __RENDER_RESOURCES_H__

#include "stdafx.h"
#include "Log.h"
#include "Render/Shader.h"
#include "Render/GBuffer.h"
#include "Render/ModelBuilder.h"

namespace engine {
    class Render;

    class RenderResources {
        friend Render;

    public:
        static void updateViewports();

    private:
        RenderResources() {}

        void init(glm::ivec2 viewport);
        void onClose();

        static RenderResources* g_selfRef;
        static bool g_isMustUpdateViewports;

        Shader* m_shaderFinal;

        GBuffer* m_gBuffer;

        Mesh* m_primitivePlane;
        Mesh* m_primitiveScreenPlane;
    };
}

#endif