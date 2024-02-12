#ifndef __SCENE__SCENE_REQUEST_H__
#define __SCENE__SCENE_REQUEST_H__

#include "../stdafx.h"
#include "../engine_properties.h"
#include "Transform.h"
#include "RenderComponent.h"

namespace engine {
    class Scene;

    class SceneRequest {
        friend Scene;
    public:
        SceneRequest();

        void addEntity(Transform& transform, RenderComponent& renderComponent);
        void deleteEntity(int id);

    private:
        struct EntityCreateRequest {
            Transform transform;
            RenderComponent renderComponent;
        };
        
        std::vector<EntityCreateRequest> m_entityCreateRequests;
        unsigned int m_entityCreateRequestsLastUnusedId{ 0 };
        std::vector<int> m_entityDeleteRequests;
        unsigned int m_entityDeleteRequestsLastUnusedId{ 0 };

        void clearEntityCreateRequests();
        void clearEntityDeleteRequests();
    };
}

#endif