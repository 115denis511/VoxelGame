#include "SphereVolume.h"

engine::SphereVolume::SphereVolume(const glm::vec3 &position, float radius) {
    m_position = position;
    m_radius = radius;
    m_area = calculateArea();
}

IsOnOrForwardPlaneResult engine::SphereVolume::isInFrustum(const Frustum &frustum, const Transform &transform) {
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

IsOnOrForwardPlaneResult engine::SphereVolume::isInFrustum(const Frustum &frustum) {
    return (isOnOrForwardPlane(frustum.getNearPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getFarPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getLeftPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getRightPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getTopPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getBottomPlane(), m_position) );
}

IsOnOrForwardPlaneResult engine::SphereVolume::isOnOrForwardPlane(const Plane &plane, const glm::vec3 &position) const {
    // Для проверки на полное нахождение, не перскающее края плоскости, нужно убрать минус у радиуса
    //return plane.signedDistanceToPlane(position) > -m_radius;

    float signedDistance = plane.signedDistanceToPlane(position);
    bool isOnOrForwardPlane = signedDistance > -m_radius;
    bool completelyInside = signedDistance > m_radius;
    return IsOnOrForwardPlaneResult(isOnOrForwardPlane, completelyInside);
}

IsOnOrForwardPlaneResult engine::SphereVolume::isOnOrForwardPlane(const Plane &plane, const glm::vec3 &position, float radius) const {
    //return plane.signedDistanceToPlane(position) > -radius;

    float signedDistance = plane.signedDistanceToPlane(position);
    bool isOnOrForwardPlane = signedDistance > -radius;
    bool completelyInside = signedDistance > radius;
    return IsOnOrForwardPlaneResult(isOnOrForwardPlane, completelyInside);
}

bool engine::SphereVolume::overlaps(const SphereVolume &other) const noexcept {
    return glm::distance(m_position, other.getPosition()) <= m_radius + other.getRadius();
}

bool engine::SphereVolume::contains(const SphereVolume &other) const noexcept {
    return glm::distance(m_position, other.getPosition()) + other.getRadius() <= m_radius;
}

engine::SphereVolume engine::SphereVolume::merge(const SphereVolume &other) const noexcept {
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

float engine::SphereVolume::calculateArea() noexcept {
    return 4 * M_PI * (m_radius * m_radius);
}