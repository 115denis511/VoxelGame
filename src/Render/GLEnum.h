#ifndef __RENDER__GL_ENUM_H__
#define __RENDER__GL_ENUM_H__

#include "../stdafx.h"

namespace engine {
    enum class BufferUsage : GLenum {
        STREAM_DRAW = GL_STREAM_DRAW, 
        STREAM_READ = GL_STREAM_READ, 
        STREAM_COPY = GL_STREAM_COPY, 
        STATIC_DRAW = GL_STATIC_DRAW, 
        STATIC_READ = GL_STATIC_READ, 
        STATIC_COPY = GL_STATIC_COPY, 
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW, 
        DYNAMIC_READ = GL_DYNAMIC_READ, 
        DYNAMIC_COPY = GL_DYNAMIC_COPY
    };
    
    enum class MapAccess : GLbitfield {
        MAP_READ_BIT = GL_MAP_READ_BIT,
        MAP_WRITE_BIT  = GL_MAP_WRITE_BIT,
        MAP_PERSISTENT_BIT = GL_MAP_PERSISTENT_BIT,
        MAP_COHERENT_BIT = GL_MAP_COHERENT_BIT
    };
}

#endif