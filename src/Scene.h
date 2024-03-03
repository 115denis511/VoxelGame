#ifndef __SCENE_H__
#define __SCENE_H__

#include "stdafx.h"
#include "engine_properties.h"
#include "Scene/Camera.h"
#include "Scene/SceneResources.h"
#include "Scene/SceneRequest.h"
#include "Scene/ISceneLogic.h"
#include "Collisions/BVHTree.h"

namespace engine {
    class Core;

    class Scene {
        friend Core;
    public:
        static bool init();
        static void freeResources();
        static SceneResources* getSceneResources();
        static void setCurrentSceneLogic(ISceneLogic* sceneLogic);
        static void sceneLogicUpdatePhase();
        static void applyChangesPhase();
        static void applyRequestsPhase();

    private:
        static Camera g_camera;
        static SceneResources* g_resouces;
        static SceneRequest* g_requests;
        static ISceneLogic* g_currentSceneLogic;
        static BVHTree g_worldBVH;

    };
}

#endif