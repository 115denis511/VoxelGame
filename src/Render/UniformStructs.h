#ifndef __RENDER__UNIFORM_STRUCTS_H__
#define __RENDER__UNIFORM_STRUCTS_H__

#include "../stdafx.h"

namespace engine {
    namespace uniform_structs {
        struct DrawVars {
            glm::mat4 projectionView;
            glm::mat4 projection;
            glm::mat4 View;
        };

        struct TexturePack {
            GLuint64 colorSpec{ 0 };
            GLuint colorSpecLayer{ 0 };
            GLuint colorSpecCapacity{ 0 };
        };
    }
}

#endif