#include "SceneRequest.h"

engine::SceneRequest::SceneRequest() {

}

void engine::SceneRequest::addEntity(Transform &transform, RenderComponent &renderComponent) {
    EntityCreateRequest request;
    request.transform = transform;
    request.renderComponent = renderComponent;

    if (m_entityCreateRequestsLastUnusedId == m_entityCreateRequests.size()) {
        m_entityCreateRequests.push_back(request);
        m_entityCreateRequestsLastUnusedId = m_entityCreateRequests.size();
    }
    else {
        m_entityCreateRequests[m_entityCreateRequestsLastUnusedId] = request;
        m_entityCreateRequestsLastUnusedId++;
    }
}

void engine::SceneRequest::deleteEntity(int id) {
    if (id < 0 || id >= (int)engine_properties::SCENE_MAX_ENTITIES) {
        Log::addMessage("WARNING! SceneRequest::deleteEntity() try to delete entity by out of range id.\nid: " + std::to_string(id));
        return;
    }

    if (m_entityDeleteRequestsLastUnusedId == m_entityDeleteRequests.size()) {
        m_entityDeleteRequests.push_back(id);
        m_entityDeleteRequestsLastUnusedId = m_entityDeleteRequests.size();
    }
    else {
        m_entityDeleteRequests[m_entityDeleteRequestsLastUnusedId] = id;
        m_entityDeleteRequestsLastUnusedId++;
    }
}

void engine::SceneRequest::clearEntityCreateRequests() {
    m_entityCreateRequestsLastUnusedId = 0;
}

void engine::SceneRequest::clearEntityDeleteRequests() {
    m_entityDeleteRequestsLastUnusedId = 0;
}