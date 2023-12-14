#include "Render.h"

bool engine::Render::init() {
    Shader s("Shader/simple.vert", "Shader/simple.frag");
    GBuffer gb(WindowGLFW::getViewport());

    if (Log::isHaveFatalError()) return false;
    return true;
}