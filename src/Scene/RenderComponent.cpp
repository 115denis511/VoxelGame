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

void engine::RenderComponent::updateModelMatrix(const glm::mat4 &matrix) {
    m_modelMatrix = matrix;
}

void engine::RenderComponent::pushMatrixToInstancingBuffer() {
    m_model->pushMatrixToInstancingBuffer(m_modelMatrix);
}

glm::mat4& engine::RenderComponent::getMatrix() {
    return m_modelMatrix;
}

engine::Model* engine::RenderComponent::getModel() {
    return m_model;
}
