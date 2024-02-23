#ifndef __COLLISIONS__FRUSTUM_H__
#define __COLLISIONS__FRUSTUM_H__

#include "../stdafx.h"
#include "../Scene/Camera.h"
#include "../Render/ProjectionPerspective.h"
#include "Plane.h"

namespace engine {
    class Frustum {
    public:
        Frustum(const CameraVars& camera, const ProjectionPerspective& projection);
        ~Frustum() {}

        const Plane& getNearPlane() const;
        const Plane& getFarPlane() const;
        const Plane& getLeftPlane() const;
        const Plane& getRightPlane() const;
        const Plane& getTopPlane() const;
        const Plane& getBottomPlane() const;

    private:
        Frustum() {}

        Plane m_nearPlane;
        Plane m_farPlane;

        Plane m_leftPlane;
        Plane m_rightPlane;

        Plane m_topPlane;
        Plane m_bottomPlane;
        
    };
}

#endif