#ifndef __RENDER_H__
#define __RENDER_H__

#include "stdafx.h"
#include "WindowGLFW.h"
#include "Render/UniformManager.h"
#include "Render/Shader.h"
#include "Render/GBuffer.h"
#include "Render/Mesh.h"
#include "Render/TextureManager.h"
#include "Render/TextureArray.h"
#include "Render/AssetManager.h"

// test, to delete
#include "Camera.h"
#include "Render/Model.h"

namespace engine {
    class Render {
    public:
        static bool init();
        static void freeResources();
        static void draw();

    private:
        static Shader*          g_shaderFinal;
        static Shader*          g_shaderMix_RGB_A;
        static Shader*          g_shaderFill_RGB;
        static GBuffer*         g_gBuffer;
        static MeshRef          g_primitiveFullScreenRect;

        static Model* test_model;

        static void updateViewports();
    };
}

#endif