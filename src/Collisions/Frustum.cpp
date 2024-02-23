#include "Frustum.h"

engine::Frustum::Frustum(const CameraVars &camera, const ProjectionPerspective &projection) {
    const float zNear = projection.getNear();
    const float zFar = projection.getFar();
    const float aspect = projection.getAspect();
    const float fovY = glm::radians(projection.getFovDegrees());
    const glm::vec3 cameraPosition = camera.cameraPosition;
    const glm::vec3 cameraFront = camera.cameraTarget;
    const glm::vec3 cameraRight = camera.cameraRight;
    const glm::vec3 cameraUp = camera.cameraUp;

    const float halfVSide = zFar * tanf(fovY * .5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar = zFar * cameraFront;

    m_nearPlane = Plane(cameraPosition + zNear * cameraFront, cameraFront);
    m_farPlane = Plane(cameraPosition + frontMultFar, -cameraFront);

    m_rightPlane = Plane(cameraPosition, glm::cross(frontMultFar - cameraRight * halfHSide, cameraUp));
    m_leftPlane = Plane(cameraPosition, glm::cross(cameraUp, frontMultFar + cameraRight * halfHSide));

    m_topPlane = Plane(cameraPosition, glm::cross(cameraRight, frontMultFar - cameraUp * halfVSide));
    m_bottomPlane = Plane(cameraPosition, glm::cross(frontMultFar + cameraUp * halfVSide, cameraRight));
}

const engine::Plane &engine::Frustum::getNearPlane() const {
    return m_nearPlane;
}

const engine::Plane &engine::Frustum::getFarPlane() const {
    return m_farPlane;
}

const engine::Plane &engine::Frustum::getLeftPlane() const {
    return m_leftPlane;
}

const engine::Plane &engine::Frustum::getRightPlane() const {
    return m_rightPlane;
}

const engine::Plane &engine::Frustum::getTopPlane() const {
    return m_topPlane;
}

const engine::Plane &engine::Frustum::getBottomPlane() const {
    return m_bottomPlane;
}