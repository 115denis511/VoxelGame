#ifndef __RENDER__VERTEX_H__
#define __RENDER__VERTEX_H__

#include "../stdafx.h"

namespace engine {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::ivec4 boneIds;
        glm::vec4 boneWeights;
    };
}

#endif