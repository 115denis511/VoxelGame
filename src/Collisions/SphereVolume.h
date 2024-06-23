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
        const glm::vec3& getPosition() const { return m_position; }
        const float getRadius() const { return m_radius; }
        const float getArea() const { return m_area; }
        bool overlaps(const SphereVolume& other) const noexcept;
        bool contains(const SphereVolume& other) const noexcept;
        SphereVolume merge(const SphereVolume& other) const noexcept;

    private:
        glm::vec3 m_position;
        float m_radius;
        float m_area;

        float calculateArea() noexcept;
    };
}

#endif