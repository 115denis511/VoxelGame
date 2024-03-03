#ifndef __COLLISIONS__AABB_H__
#define __COLLISIONS__AABB_H__

#include "../stdafx.h"
#include "Frustum.h"
#include "../Scene/Transform.h"

namespace engine {
    class AABB {
    public:
        AABB(const glm::vec3& min = glm::vec3(0.f), const glm::vec3& max = glm::vec3(0.f));
        AABB(const glm::vec3& position, float eXYZ);
        AABB(const glm::vec3& position, float eX, float eY, float eZ);
        ~AABB();

        bool isInFrustum(const Frustum& frustum, const Transform &transform);
        bool isInFrustum(const Frustum& frustum);
        bool isOnOrForwardPlane(const Plane& plane, const glm::vec3& position) const;
        const glm::vec3& getMin() const { return m_min; }
        const glm::vec3& getMax() const { return m_max; }
        float getArea() const { return m_area; }
        AABB merge(const AABB& other);

        inline const glm::vec3& getPosition() const { return m_position; }
        inline const glm::vec3& getExtents() const { return m_extents; }

    private:
        glm::vec3 m_position;
        glm::vec3 m_extents;
        glm::vec3 m_min;
        glm::vec3 m_max;
        float     m_area;

        float calculateArea();
    };
}

#endif