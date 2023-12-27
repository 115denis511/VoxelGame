#ifndef __RENDER_H__
#define __RENDER_H__

#include "stdafx.h"
#include "WindowGLFW.h"
#include "Render/Shader.h"
#include "Render/GBuffer.h"
#include "Render/Mesh.h"
#include "Render/TextureManager.h"
#include "Render/TextureArray.h"

namespace engine {
    class Render {
    public:
        static bool init();
        static void freeResources();
        static void draw();

    private:
        static Shader*  g_shaderFinal;
        static Shader*  g_shaderMix_RGB_A;
        static GBuffer* g_gBuffer;
        static Mesh*    g_primitivePlane;
        static Mesh*    g_primitiveScreenPlane;

        static void updateViewports();
        static Mesh* buildPrimitivePlane(GLfloat leftX, GLfloat topY, GLfloat rightX, GLfloat bottomY);
    };
}

#endif