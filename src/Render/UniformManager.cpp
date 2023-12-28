#include "UniformManager.h"

engine::UniformBuffer* engine::UniformManager::g_drawVars;

void engine::UniformManager::setDrawVars(uniform_structs::DrawVars &vars) {
    glNamedBufferSubData(g_drawVars->getUBO(), 0, sizeof(vars), &vars);
}

bool engine::UniformManager::init() {
    g_drawVars = new UniformBuffer(
        engine_properties::UNIFORM_BUFFER_DRAW_VARS_BLOCK_ID,
        engine_properties::UNIFORM_BUFFER_DRAW_VARS_SIZE
    );

    return true;
}

void engine::UniformManager::freeResources() {
    delete g_drawVars;
}
