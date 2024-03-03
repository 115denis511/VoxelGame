#ifndef __SCENE__ENTITY_REFERENCES_H__
#define __SCENE__ENTITY_REFERENCES_H__

namespace engine {
    class Scene;
    class BVHTree;

    class EntityReferences {
        friend Scene;
        friend BVHTree;
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
        unsigned int m_BVHNodeId{ 0xffffffff };
    };
}

#endif