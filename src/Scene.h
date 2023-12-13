#ifndef __SCENE_H__
#define __SCENE_H__

#include "stdafx.h"
#include "engine_properties.h"
#include "SceneResources.h"

namespace engine {
    class Scene {
    public:
        static bool init();
        static void onClose();

    private:
        static SceneResources* g_resouces;

    };
}

#endif