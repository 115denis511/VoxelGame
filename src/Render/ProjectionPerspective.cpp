#include "ProjectionPerspective.h"

engine::ProjectionPerspective::ProjectionPerspective(const glm::ivec2 viewpoint, GLfloat fovDegrees, GLfloat zNear, GLfloat zFar) {
    update(viewpoint, fovDegrees, zNear, zFar);
}

engine::ProjectionPerspective::~ProjectionPerspective() {

}

void engine::ProjectionPerspective::update(const glm::ivec2 viewpoint, GLfloat fovDegrees, GLfloat zNear, GLfloat zFar) {
    m_fovDegrees = fovDegrees;
    m_zNear = zNear;
    m_zFar = zFar;
    m_aspect = (float)viewpoint.x / (float)viewpoint.y;

    m_projectionMatrix = glm::perspective(glm::radians(m_fovDegrees), m_aspect, m_zNear, m_zFar);
}

void engine::ProjectionPerspective::update(const glm::ivec2 viewpoint) {
    m_aspect = (float)viewpoint.x / (float)viewpoint.y;

    m_projectionMatrix = glm::perspective(glm::radians(m_fovDegrees), m_aspect, m_zNear, m_zFar);
}
