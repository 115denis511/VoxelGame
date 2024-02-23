#ifndef __RENDER__PROJECTION_PERSPECTIVE_H__
#define __RENDER__PROJECTION_PERSPECTIVE_H__

#include "../stdafx.h"

namespace engine {
    class ProjectionPerspective {
    public:
        ProjectionPerspective(const glm::ivec2 viewpoint = glm::ivec2(1300.f,700.f), GLfloat fovDegrees = 45.f, GLfloat zNear = 0.1f, GLfloat zFar = 1000.f);
        ~ProjectionPerspective();

        void update(const glm::ivec2 viewpoint, GLfloat fovDegrees, GLfloat zNear, GLfloat zFar);
        void update(const glm::ivec2 viewpoint);
        const glm::mat4& getMatrix() const { return m_projectionMatrix; }
        const GLfloat getFovDegrees() const { return m_fovDegrees; }
        const GLfloat getNear() const { return m_zNear; }
        const GLfloat getFar() const { return m_zFar; }
        const GLfloat getAspect() const { return m_aspect; }

    private:
        glm::mat4 m_projectionMatrix;
        GLfloat m_fovDegrees;
        GLfloat m_zNear;
        GLfloat m_zFar;
        GLfloat m_aspect;
    };
}

#endif