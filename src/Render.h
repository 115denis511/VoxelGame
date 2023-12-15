#ifndef __RENDER_H__
#define __RENDER_H__

#include "stdafx.h"
#include "WindowGLFW.h"
#include "RenderResources.h"

namespace engine {
    class Render {
    public:
        static bool init();
        static void onClose();
        static void draw();

    private:
        static RenderResources* g_resources;

    };
}

#endif