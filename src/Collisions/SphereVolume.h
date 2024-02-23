#ifndef __COLLISIONS__SPHERE_VOLUME_H__
#define __COLLISIONS__SPHERE_VOLUME_H__

#include "../stdafx.h"
#include "../Scene/Transform.h"
#include "Frustum.h"

namespace engine {
    class SphereVolume {
    public:
        SphereVolume(const glm::vec3& position = glm::vec3(0.f), float radius = 0.f);

        bool isInFrustum(const Frustum& frustum, const Transform &transform);
        bool isInFrustum(const Frustum& frustum);
        bool isOnOrForwardPlane(const Plane& plane, const glm::vec3& position) const;
        bool isOnOrForwardPlane(const Plane& plane, const glm::vec3& position, float radius) const;
        const float getRadius() { return m_radius; }

    private:
        glm::vec3 m_position;
        float m_radius;
    };
}

#endif