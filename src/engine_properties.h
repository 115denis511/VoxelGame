#ifndef __ENGINE_PROPERTIES_H__
#define __ENGINE_PROPERTIES_H__

#include "Render/UniformStructs.h"

namespace engine_properties {
    // Scene
    constexpr size_t SCENE_MAX_ENTITIES = 4096;
    constexpr size_t SCENE_MAX_TRANSFORMS = 4096;
    constexpr size_t SCENE_MAX_RENDER_COMPONENTS = 4096;

    // Uniform buffers
    constexpr GLuint     UNIFORM_BUFFER_DRAW_VARS_BLOCK_ID = 0;
    constexpr GLsizeiptr UNIFORM_BUFFER_DRAW_VARS_SIZE = sizeof(engine::uniform_structs::DrawVars);
    constexpr GLuint     UNIFORM_BUFFER_TEXTURE_PACK_BLOCK_ID = 1;
    constexpr GLsizeiptr UNIFORM_BUFFER_TEXTURE_PACK_SIZE = sizeof(engine::uniform_structs::TexturePack);

    // Textures
    constexpr GLuint TEXTURE_ARRAY_W64_H64_SIZE = 1024;
    constexpr GLuint TEXTURE_ARRAY_W512_H512_SIZE = 128;
    constexpr GLuint TEXTURE_ARRAY_W1024_H1024_SIZE = 64;
    constexpr GLsizei MIPMAP_LEVELS = 6;

    // Log
    constexpr size_t MAX_LOG_STRINGS = 2048;
}

#endif