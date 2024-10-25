#ifndef __SCENE__ENTITY_REFERENCES_H__
#define __SCENE__ENTITY_REFERENCES_H__

#include "../Collisions/BVHTree.h"
#include "../Collisions/SphereVolume.h"

namespace engine {
    class Scene;

    class EntityReferences : public IBVHTreeItem<EntityReferences, SphereVolume> {
        friend Scene;
    public:
        EntityReferences(int transformId = -1,
                         int renderComponentId = -1);

        const int getId() const;
        const int getTransformId() const;
        const int getRenderComponentId() const;


    private:
        int m_id{ -1 };
        int m_transformId{ -1 };
        int m_renderComponentId{ -1 };
    };
}

#endif