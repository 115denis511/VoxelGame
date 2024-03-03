#include "AABB.h"

engine::AABB::AABB(const glm::vec3 &min, const glm::vec3 &max) {
    m_position = glm::vec3((max + min) * 0.5f);
    m_extents = glm::vec3(max.x - m_position.x, max.y - m_position.y, max.z - m_position.z);
    m_min = min;
    m_max = max;
    m_area = calculateArea();
}

engine::AABB::AABB(const glm::vec3& position, float eXYZ) {
    m_position = position;
    m_extents = glm::vec3(eXYZ);
    m_min = m_position - m_extents;
    m_max = m_position + m_extents;
    m_area = calculateArea();
}

engine::AABB::AABB(const glm::vec3& position, float eX, float eY, float eZ) {
    m_position = position;
    m_extents = glm::vec3(eX, eY, eZ);
    m_min = m_position - m_extents;
    m_max = m_position + m_extents;
    m_area = calculateArea();
}

engine::AABB::~AABB() {

}

bool engine::AABB::isInFrustum(const Frustum &frustum, const Transform &transform) {
    glm::vec3 transformedPosition = transform.getModelMatrix() * glm::vec4(m_position, 1.f);

    const glm::vec3 right = transform.getRight() * m_extents.x;
    const glm::vec3 up = transform.getUp() * m_extents.y;
    const glm::vec3 forward = transform.getFront() * m_extents.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    AABB appliedTransform(transformedPosition, newIi, newIj, newIk);

    return (appliedTransform.isOnOrForwardPlane(frustum.getNearPlane(), transformedPosition) &&
            appliedTransform.isOnOrForwardPlane(frustum.getFarPlane(), transformedPosition) &&
            appliedTransform.isOnOrForwardPlane(frustum.getLeftPlane(), transformedPosition) &&
            appliedTransform.isOnOrForwardPlane(frustum.getRightPlane(), transformedPosition) &&
            appliedTransform.isOnOrForwardPlane(frustum.getTopPlane(), transformedPosition) &&
            appliedTransform.isOnOrForwardPlane(frustum.getBottomPlane(), transformedPosition) );
}

bool engine::AABB::isInFrustum(const Frustum &frustum) {
    return (isOnOrForwardPlane(frustum.getNearPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getFarPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getLeftPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getRightPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getTopPlane(), m_position) &&
            isOnOrForwardPlane(frustum.getBottomPlane(), m_position) );
}

bool engine::AABB::isOnOrForwardPlane(const Plane &plane, const glm::vec3& position) const {
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = m_extents.x * std::abs(plane.getNormal().x) +
            m_extents.y * std::abs(plane.getNormal().y) + m_extents.z * std::abs(plane.getNormal().z);

    return -r <= plane.signedDistanceToPlane(position);
}

engine::AABB engine::AABB::merge(const AABB &other) {
    return AABB(glm::vec3(std::min(m_min.x, other.getMin().x), std::min(m_min.y, other.getMin().y), std::min(m_min.z, other.getMin().z)),
                glm::vec3(std::max(m_min.x, other.getMax().x), std::max(m_min.y, other.getMax().y), std::max(m_min.z, other.getMax().z)));
}

float engine::AABB::calculateArea() {
    glm::vec3 size = m_max - m_min;
    return 2.0f * (size.x * size.y + size.y * size.z + size.z * size.x);
}
