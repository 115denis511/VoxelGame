#ifndef __COLLISIONS__BVH_TREE_ENTITIES_H__
#define __COLLISIONS__BVH_TREE_ENTITIES_H__

#include "../stdafx.h"
#include "../Scene/EntityReferences.h"
#include "../Scene/RenderComponent.h"
#include "Frustum.h"
#include "BVHTree.h"

namespace engine {
    class BVHTreeEntities : public BVHTree<EntityReferences> {
    public:
        BVHTreeEntities() {}

        std::vector<int> getOverlapsedRenderComponents(Frustum frustum);
    };
}

#endif