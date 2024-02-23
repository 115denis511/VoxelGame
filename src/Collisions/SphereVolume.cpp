#include "SphereVolume.h"

engine::SphereVolume::SphereVolume(const glm::vec3 &position, float radius) {
    m_position = position;
    m_radius = radius;
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
