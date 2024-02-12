#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "stdafx.h"
#include "engine_properties.h"
#include "WindowGLFW.h"
#include "Controls.h"
#include "Scene.h"
#include "Render.h"
#include "Log.h"

namespace engine {

    class Core {
    public:
        static bool init();
        static void start(ISceneLogic* sceneLogic);
        static void close();
        static void onFatalError();

    private:
        Core() {}
    };
}

#endif //!__ENGINE_H__