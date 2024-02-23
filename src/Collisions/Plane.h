#ifndef __COLLISIONS__PLANE_H__
#define __COLLISIONS__PLANE_H__

#include "../stdafx.h"

namespace engine {
    class Frustum;

    class Plane {
        friend Frustum;

    public:
        Plane(glm::vec3 normal, float distance);
        Plane(const glm::vec3 &position, const glm::vec3 &norm);
        ~Plane();

        const float getDistance() const;
        const glm::vec3& getNormal() const;
        float signedDistanceToPlane(const glm::vec3& position) const;

    private:
        Plane() {}

        glm::vec3 m_normal;
        float m_distance;
    };
}

#endif