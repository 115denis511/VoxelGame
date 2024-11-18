#ifndef __RENDER_H__
#define __RENDER_H__

#include "stdafx.h"
#include "WindowGLFW.h"
#include "Render/UniformManager.h"
#include "Render/ShaderStorageManager.h"
#include "Render/Shader.h"
#include "Render/GBuffer.h"
#include "Render/Mesh.h"
#include "Render/AssetManager.h"
#include "Render/ProjectionPerspective.h"
#include "Scene/Camera.h"
#include "Scene/SceneResources.h"
#include "Collisions/Frustum.h"
#include "Collisions/BVHTreeEntities.h"
#include "Voxel/MarchingCubesManager.h"

// test, to delete
#include "Voxel/VoxelChunk.h"

/*
        ВНИМАНИЕ!!!
        УЧЕСТЬ ПРИ РАСПАРАЛЛЕЛИВАНИИ, ЧТО КЛАСС SCENE ДЕЛАЕТ ВЫЗОВЫ OPENGL В
        МЕТОДАХ applyChangesPhase И applyRequestsPhase, А ИМЕННО МЕТОДЫ
        ShaderStorageManager::getMappedTransformsSSBO() И 
        ShaderStorageManager::unmapTransformsSSBO()
*/

namespace engine {
    class Render {
    public:
        ~Render();

        static Render* getInstance() { return g_instance; }
        static bool init();
        static void freeResources();

        void draw(CameraVars cameraVars, SceneResources& sceneResources, BVHTreeEntities& worldBVH);

    private:
        Render(bool& hasError);

        static Render* g_instance;

        Shader*                  m_shaderFinal;
        Shader*                  m_shaderMix_RGB_A;
        Shader*                  m_shaderFill_RGB;
        Shader*                  m_shaderMarchingCubes;
        Shader*                  m_shaderMarchingCubesPrecomp;
        Shader*                  m_shaderComputeMarchingCubes;
        GBuffer*                 m_gBuffer;
        MeshRef                  m_primitiveFullScreenRect;
        ProjectionPerspective    m_projectionPerspective;
        std::vector<Model*>      m_modelsToInstancedDraw;

        Model* test_model;

        void updateViewports();
        void accamulateInstancingBuffers(SceneResources& sceneResources, BVHTreeEntities& worldBVH, Frustum frustum);
        void drawInstanced();
        void addToInstancedDrawList(Model* model);
    };
}

#endif