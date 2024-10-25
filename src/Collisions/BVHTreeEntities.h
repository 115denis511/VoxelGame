#ifndef __COLLISIONS__BVH_TREE_ENTITIES_H__
#define __COLLISIONS__BVH_TREE_ENTITIES_H__

#include "../stdafx.h"
#include "../Scene/EntityReferences.h"
#include "../Scene/RenderComponent.h"
#include "Frustum.h"
#include "BVHTree.h"
#include "SphereVolume.h"

namespace engine {
    class BVHTreeEntities : public BVHTree<EntityReferences, SphereVolume> {
    public:
        BVHTreeEntities() {}

        std::vector<int> getOverlapsedRenderComponents(Frustum frustum);

    private:
        void nodeProcess(unsigned int nodeId, std::vector<int>& overlaps, const Frustum& frustum);
    };
}

#endif