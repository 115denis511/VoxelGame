#ifndef __RENDER_H__
#define __RENDER_H__

#include "stdafx.h"
#include "WindowGLFW.h"
#include "Render/UniformManager.h"
#include "Render/InstancingManager.h"
#include "Render/Shader.h"
#include "Render/GBuffer.h"
#include "Render/Mesh.h"
#include "Render/AssetManager.h"
#include "Render/ProjectionPerspective.h"
#include "Scene/Camera.h"
#include "Scene/SceneResources.h"
#include "Collisions/Frustum.h"

// test, to delete
#include "Render/Model.h"

namespace engine {
    class Render {
    public:
        static bool init();
        static void freeResources();
        static void draw(CameraVars cameraVars, SceneResources& sceneResources);

    private:
        static Shader*                  g_shaderFinal;
        static Shader*                  g_shaderMix_RGB_A;
        static Shader*                  g_shaderFill_RGB;
        static GBuffer*                 g_gBuffer;
        static MeshRef                  g_primitiveFullScreenRect;
        static ProjectionPerspective    g_projectionPerspective;
        static std::vector<Model*>      g_modelsToInstancedDraw;

        static Model* test_model;

        static void updateViewports();
        static void accamulateInstancingBuffers(SceneResources& sceneResources, Frustum frustum);
        static void drawInstanced();
        static void addToInstancedDrawList(Model* model);
    };
}

#endif