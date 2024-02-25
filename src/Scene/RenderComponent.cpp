#include "RenderComponent.h"

engine::RenderComponent::RenderComponent() {
    m_model = nullptr;
}

engine::RenderComponent::RenderComponent(Model *model) {
    m_model = model;
}

void engine::RenderComponent::draw() {
    m_model->draw();
}

void engine::RenderComponent::pushTransformIdToInstancingBuffer() {
    m_model->pushTransformIdToInstancingBuffer(m_transformId);
}

const engine::Transform &engine::RenderComponent::getTransform() {
    return *m_transform;
}

engine::Model *engine::RenderComponent::getModel() {
    return m_model;
}

void engine::RenderComponent::setTransform(Transform *transform) {
    m_transform = transform;
    m_transformId = m_transform->getId(); 
}
