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
        RenderResources(glm::ivec2 viewport);
        ~RenderResources();

    private:
        Shader* m_shaderFinal;

        GBuffer* m_gBuffer;

        Mesh* m_primitivePlane;
        Mesh* m_primitiveScreenPlane;
    };
}

#endif