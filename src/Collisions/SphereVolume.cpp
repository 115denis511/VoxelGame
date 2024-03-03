#include "SphereVolume.h"

engine::SphereVolume::SphereVolume(const glm::vec3 &position, float radius) {
    m_position = position;
    m_radius = radius;
    m_area = calculateArea();
}

bool engine::SphereVolume::isInFrustum(const Frustum &frustum, const Transform &transform) {
    glm::vec3 position = transform.getModelMatrix() * glm::vec4(m_position, 1.f);
    const glm::vec3& scale = transform.getScale();
    float radius = m_radius * std::max(scale.x, std::max(scale.y, scale.z));

    return (isOnOrForwardPlane(frustum.getNearPlane(), position, radius) &&
            isOnOrForwardPlane(frustum.getFarPlane(), position, radius) &&
            isOnOrForwardPlane(frustum.getLeftPlane(), position, radius) &&
            isOnOrForwardPlane(frustum.getRightPlane(), position, radius) &&
            isOnOrForwardPlane(frustum.getTopPlane(), position, radius) &&
            isOnOrForwardPlane(frustum.getBottomPlane(), position, radius) );
}

bool engine::SphereVolume::isInFrustum(const Frustum &frustum) {
    return (isOnOrForwardPlane(frustum.getNearPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getFarPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getLeftPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getRightPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getTopPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getBottomPlane(), m_position) );
}

bool engine::SphereVolume::isOnOrForwardPlane(const Plane &plane, const glm::vec3 &position) const {
    return plane.signedDistanceToPlane(position) > -m_radius;
}

bool engine::SphereVolume::isOnOrForwardPlane(const Plane &plane, const glm::vec3 &position, float radius) const {
    return plane.signedDistanceToPlane(position) > -radius;
}

bool engine::SphereVolume::overlaps(const SphereVolume &other) const {
    return glm::distance(m_position, other.getPosition()) <= m_radius + other.getRadius();
}

bool engine::SphereVolume::contains(const SphereVolume &other) const {
    return glm::distance(m_position, other.getPosition()) + other.getRadius() <= m_radius;
}

engine::SphereVolume engine::SphereVolume::merge(const SphereVolume &other) const {
    // https://stackoverflow.com/questions/33532860/merge-two-spheres-to-get-a-new-one

    // Если один из объёмов содержит внутри себя другой, то вернуть больший объём
    if (contains(other)) {
        return *this;
    }
    if (other.contains(*this)) {
        return other;
    }

    // Иначе вычисляем объём, содержащий в себе 2 других
    float radius = (m_radius + other.getRadius() + glm::distance(m_position, other.getPosition())) * 0.5;
    glm::vec3 position = m_position + (other.getPosition() - m_position) * (radius - m_radius) / glm::distance(other.getPosition(), m_position);
    // Деления на ноль не будет, так как если объёмы находятся друг на друге, то один из них перекрывает другой и выполнится return выше

    return SphereVolume(position, radius);
}

float engine::SphereVolume::calculateArea() {
    return 4 * M_PI * (m_radius * m_radius);
}