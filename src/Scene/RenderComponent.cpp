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

glm::mat4 &engine::RenderComponent::getMatrix() {
    return m_modelMatrix;
}
