#ifndef __RENDER__UNIFORM_STRUCTS_H__
#define __RENDER__UNIFORM_STRUCTS_H__

#include "../stdafx.h"

namespace engine {
    namespace uniform_structs {
        struct DrawVars {
            glm::mat4 projectionView;
        };
    }
}

#endif