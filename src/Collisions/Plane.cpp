#include "Plane.h"

engine::Plane::Plane(glm::vec3 normal, float distance) {
    m_normal = normal;
    m_distance = distance;
}

engine::Plane::Plane(const glm::vec3& position, const glm::vec3& norm){
    m_normal = glm::normalize(norm);
    m_distance = glm::dot(m_normal, position);
}

engine::Plane::~Plane() {

}

const float engine::Plane::getDistance() const {
    return m_distance;
}

const glm::vec3& engine::Plane::getNormal() const {
    return m_normal;
}

float engine::Plane::signedDistanceToPlane(const glm::vec3 &position) const {
    return glm::dot(m_normal, position) - m_distance;
}
