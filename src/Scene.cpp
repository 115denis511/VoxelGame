#include "Scene.h"

engine::Camera          engine::Scene::g_camera;
engine::SceneResources* engine::Scene::g_resouces = nullptr;
engine::SceneRequest*   engine::Scene::g_requests = nullptr;
engine::ISceneLogic*    engine::Scene::g_currentSceneLogic = nullptr;

bool engine::Scene::init() {
    g_resouces = new SceneResources();
    g_requests = new SceneRequest();

    g_camera.updateLookAt();

    return true;
}

void engine::Scene::freeResources() {
    delete g_resouces;
    delete g_requests;
}

engine::SceneResources *engine::Scene::getSceneResources() {
    return g_resouces;
}

void engine::Scene::setCurrentSceneLogic(ISceneLogic *sceneLogic) {
    g_currentSceneLogic = sceneLogic;
}

void engine::Scene::sceneLogicUpdatePhase() {
    g_currentSceneLogic->update(g_camera, *g_resouces);
}

void engine::Scene::applyChangesPhase() {
    g_camera.updateLookAt();

    for (int i = 0; i <= g_resouces->transforms.getBiggestUsedId(); i++) {
        auto& object = g_resouces->transforms.get(i);
        if (object.isInUse() && object.getObject().isNeedToUpdateMatrix()) {
            object.getObject().updateModelMatrix();
        }
    }
}

void engine::Scene::applyRequestsPhase() {
    // Добавление сущностей
    for (size_t i = 0; i < g_requests->m_entityCreateRequestsLastUnusedId; i++) {
        if(!g_resouces->transforms.isHaveSpace() || 
           !g_resouces->renderComponents.isHaveSpace()) {

            Log::addMessage("WARNING! Scene::applyRequestsPhase() Can't add entity - one of component buffers don't have space");
            break;
        }

        auto& transform = g_requests->m_entityCreateRequests[i].transform;
        if (transform.isNeedToUpdateMatrix())
            transform.updateModelMatrix();
        
        g_requests->m_entityCreateRequests[i].renderComponent.updateModelMatrix(transform.getModelMatrix());

        EntityReferences entity;
        g_resouces->transforms.add(g_requests->m_entityCreateRequests[i].transform, entity.m_transformId);
        g_resouces->renderComponents.add(g_requests->m_entityCreateRequests[i].renderComponent, entity.m_renderComponentId);
        
        int entityId;
        g_resouces->entities.add(entity, entityId);
        g_resouces->entities.get(entityId).getObject().m_id = entityId;
    }
    g_requests->clearEntityCreateRequests();

    // Удаление сущностей
    for (size_t i = 0; i < g_requests->m_entityDeleteRequestsLastUnusedId; i++) {
        int id = g_requests->m_entityDeleteRequests[i];
        EntityReferences& entity = g_resouces->entities.get(id).getObject();
        if (g_resouces->entities.get(id).isInUse()) {
            g_resouces->transforms.remove(entity.m_transformId);
        }
    }
    for (size_t i = 0; i < g_requests->m_entityDeleteRequestsLastUnusedId; i++) {
        int id = g_requests->m_entityDeleteRequests[i];
        EntityReferences& entity = g_resouces->entities.get(id).getObject();
        if (g_resouces->entities.get(id).isInUse()) {
            g_resouces->renderComponents.remove(entity.m_renderComponentId);
        }
    }
    // Связи сущностей чистятся всегда в конце! И если делать паралелизациию, то очистка должна быть
    // после удаления всех прочих компонентов и быть в одном потоке!!!
    for (size_t i = 0; i < g_requests->m_entityDeleteRequestsLastUnusedId; i++) {
        int id = g_requests->m_entityDeleteRequests[i];
        EntityReferences& entity = g_resouces->entities.get(id).getObject();
        if (g_resouces->entities.get(id).isInUse()) {
            g_resouces->entities.remove(entity.m_id);
        }
    }
    g_requests->clearEntityDeleteRequests();
}