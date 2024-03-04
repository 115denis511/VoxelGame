#include "Scene.h"

engine::Camera          engine::Scene::g_camera;
engine::SceneResources* engine::Scene::g_resouces = nullptr;
engine::SceneRequest*   engine::Scene::g_requests = nullptr;
engine::ISceneLogic*    engine::Scene::g_currentSceneLogic = nullptr;
engine::BVHTree         engine::Scene::g_worldBVH;

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

    glm::mat4* ssboTransforms = ShaderStorageManager::getMappedTransformsSSBO(); // <--- УЧЕСТЬ ПРИ РАСПАРАЛЛЕЛИВАНИИ
    for (int i = 0; i <= g_resouces->transforms.getBiggestUsedId(); i++) {
        auto& object = g_resouces->transforms.get(i);
        if (object.isInUse() && object.getObject().isNeedToUpdateMatrix()) {
            Transform& transform = object.getObject();

            transform.updateModelMatrix();
            glm::mat4& transformInGPU = ssboTransforms[i];
            transformInGPU = transform.getModelMatrix(); // <--- УЧЕСТЬ ПРИ РАСПАРАЛЛЕЛИВАНИИ

            EntityReferences& entityRef = g_resouces->entities.get(object.getObject().getParentId()).getObject();
            RenderComponent& renderComponent = g_resouces->renderComponents.get(entityRef.m_renderComponentId).getObject();
            SphereVolume volume = renderComponent.getModel()->getVolume(transform);
            g_worldBVH.updateObject(entityRef, volume);
        }
    }
    ShaderStorageManager::unmapTransformsSSBO();
}

void engine::Scene::applyRequestsPhase() {
    // Добавление сущностей
    glm::mat4* ssboTransforms = ShaderStorageManager::getMappedTransformsSSBO(); // <--- УЧЕСТЬ ПРИ РАСПАРАЛЛЕЛИВАНИИ
    for (size_t i = 0; i < g_requests->m_entityCreateRequestsLastUnusedId; i++) {
        if(!g_resouces->transforms.isHaveSpace() || 
           !g_resouces->renderComponents.isHaveSpace()) {

            Log::addMessage("WARNING! Scene::applyRequestsPhase() Can't add entity - one of component buffers don't have space");
            break;
        }

        auto& transform = g_requests->m_entityCreateRequests[i].transform;
        if (transform.isNeedToUpdateMatrix())
            transform.updateModelMatrix();

        // Создание объеката для хранения id компонентов сущности
        int entityId;
        g_resouces->entities.add(EntityReferences(), entityId);
        EntityReferences& entityRef = g_resouces->entities.get(entityId).getObject();
        entityRef.m_id = entityId;

        // Трансформации сущности
        g_resouces->transforms.add(g_requests->m_entityCreateRequests[i].transform, entityRef.m_transformId);
        Transform& createdTransform = g_resouces->transforms.get(entityRef.m_transformId).getObject();
        createdTransform.setId(entityRef.m_transformId);
        createdTransform.setParentId(entityId);
        glm::mat4& transformInGPU = ssboTransforms[entityRef.m_transformId];
        transformInGPU = createdTransform.getModelMatrix();

        // Компонент рендеринга сущности
        g_resouces->renderComponents.add(g_requests->m_entityCreateRequests[i].renderComponent, entityRef.m_renderComponentId);
        auto& createdRenderComponent = g_resouces->renderComponents.get(entityRef.m_renderComponentId).getObject();
        createdRenderComponent.setTransform(&createdTransform);
        
        // Вставка сущности в BVH
        g_worldBVH.insertObject(&entityRef,
                                createdRenderComponent.getModel()->getVolume(createdTransform));
    }
    ShaderStorageManager::unmapTransformsSSBO(); // <--- УЧЕСТЬ ПРИ РАСПАРАЛЛЕЛИВАНИИ
    g_requests->clearEntityCreateRequests();

    // Удаление сущностей
    // TODO: Распарелелить.
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

        if (g_resouces->entities.get(id).isInUse()) {
            EntityReferences& entity = g_resouces->entities.get(id).getObject();
            g_worldBVH.removeObject(&entity);
            g_resouces->entities.remove(entity.m_id);
        }
    }
    g_requests->clearEntityDeleteRequests();
}