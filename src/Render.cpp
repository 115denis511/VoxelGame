#include "Render.h"

bool engine::Render::init() {
    Shader s("Shader/simple.vert", "Shader/simple.frag");

    if (Log::isHaveFatalError()) return false;
    return true;
}