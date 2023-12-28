#ifndef __ENGINE_PROPERTIES_H__
#define __ENGINE_PROPERTIES_H__

#include "Render/UniformStructs.h"

namespace engine_properties {
    // Uniform buffers
    constexpr GLuint UNIFORM_BUFFER_DRAW_VARS_BLOCK_ID = 0;
    constexpr GLsizeiptr UNIFORM_BUFFER_DRAW_VARS_SIZE = sizeof(engine::uniform_structs::DrawVars);

    // Textures
    constexpr GLuint TEXTURE_ARRAY_W512_H512_SIZE = 128;
    constexpr GLuint TEXTURE_ARRAY_W1024_H1024_SIZE = 64;
    constexpr GLsizei MIPMAP_LEVELS = 6;

    // Log
    constexpr unsigned int MAX_LOG_STRINGS = 2048;
}

#endif