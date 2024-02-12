#include "Transform.h"

engine::Transform::Transform() {
    m_position = glm::vec3(0.f);
    m_rotation = glm::vec3(0.f);
    m_scale = glm::vec3(1.f);
    m_isNeedToUpdateMatrix = true;
    m_isScaled = false;

    updateModelMatrix();
}

void engine::Transform::updateModelMatrix() {
    m_isNeedToUpdateMatrix = false;

    glm::mat4 rotation = glm::yawPitchRoll(m_rotation.x, m_rotation.y, m_rotation.z);
    m_modelMatrix = glm::translate(glm::mat4(1.f), m_position) * rotation;
    if (m_isScaled) {
        m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
    }
}

const glm::mat4& engine::Transform::getModelMatrix() {
    return m_modelMatrix;
}

bool engine::Transform::isNeedToUpdateMatrix() {
    return m_isNeedToUpdateMatrix;
}

void engine::Transform::setPosition(const glm::vec3 &position) {
    m_position = position;
    m_isNeedToUpdateMatrix = true;
}

void engine::Transform::movePosition(const glm::vec3 &offset) {
    m_position += offset;
    m_isNeedToUpdateMatrix = true;
}

const glm::vec3& engine::Transform::getPosition() const {
    return m_position;
}

void engine::Transform::setRotation(const glm::vec3 &yawPitchRoll) {
    m_rotation.x = glm::radians(yawPitchRoll.x);
    m_rotation.y = glm::radians(yawPitchRoll.y);
    m_rotation.z = glm::radians(yawPitchRoll.z);
    m_isNeedToUpdateMatrix = true;
}

void engine::Transform::moveRotation(const glm::vec3 &offsetYawPitchRoll) {
    m_rotation += offsetYawPitchRoll;
    m_isNeedToUpdateMatrix = true;
}

const glm::vec3 &engine::Transform::getRotation() const {
    return m_rotation;
}

void engine::Transform::setScale(glm::vec3 scale) {
    m_scale = scale;

    m_isNeedToUpdateMatrix = true;

    if (scale.x == 1.f && scale.y == 1.f && scale.z == 1.f) {
        m_isScaled = false;
    }
    else {
        m_isScaled = true;
    }
}

const glm::vec3 &engine::Transform::getScale() const {
    return m_scale;
}
