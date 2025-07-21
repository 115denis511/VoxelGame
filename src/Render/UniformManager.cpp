#include "UniformManager.h"

engine::UniformBuffer* engine::UniformManager::g_drawVars;
engine::UniformBuffer* engine::UniformManager::g_texturePack;
engine::UniformBuffer* engine::UniformManager::g_chunkPositions;

void engine::UniformManager::setDrawVars(uniform_structs::DrawVars &vars) {
    glNamedBufferSubData(g_drawVars->getUBO(), 0, sizeof(vars), &vars);
}

void engine::UniformManager::setTexturePack(uniform_structs::TexturePack &vars) {
    glNamedBufferSubData(g_texturePack->getUBO(), 0, sizeof(vars), &vars);
}

void engine::UniformManager::setChunkPositions(const std::array<glm::vec4, 14>& positions) {
    glNamedBufferSubData(g_chunkPositions->getUBO(), 0, sizeof(positions), &positions);
}

bool engine::UniformManager::init() {
    g_drawVars = new UniformBuffer(
        engine_properties::UNIFORM_BUFFER_DRAW_VARS_BLOCK_ID,
        engine_properties::UNIFORM_BUFFER_DRAW_VARS_SIZE
    );
    g_texturePack = new UniformBuffer(
        engine_properties::UNIFORM_BUFFER_TEXTURE_PACK_BLOCK_ID,
        engine_properties::UNIFORM_BUFFER_TEXTURE_PACK_SIZE
    );
    g_chunkPositions = new UniformBuffer(
        engine_properties::UNIFORM_BUFFER_CHUNK_POSITIONS_ID,
        engine_properties::UNIFORM_BUFFER_CHUNK_POSITIONS_SIZE
    );

    return true;
}

void engine::UniformManager::freeResources() {
    delete g_drawVars;
    delete g_texturePack;
    delete g_chunkPositions;
}
