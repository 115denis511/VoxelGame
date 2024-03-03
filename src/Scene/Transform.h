#ifndef __SCENE__TRANSFORM_H__
#define __SCENE__TRANSFORM_H__

#include "../stdafx.h"

namespace engine {
    class Scene;

    class Transform {
        friend Scene;

    public:
        Transform();

        void updateModelMatrix();
        const int getId();
        const int getParentId();
        const glm::mat4& getModelMatrix() const;
        bool isNeedToUpdateMatrix();
        void setPosition(const glm::vec3& position);
        void movePosition(const glm::vec3& offset);
        const glm::vec3& getPosition() const;
        void setRotation(const glm::vec3& yawPitchRoll);
        void moveRotation(const glm::vec3& offsetYawPitchRoll);
        const glm::vec3& getRotation() const;
        void setScale(glm::vec3 scale);
        const glm::vec3& getScale() const;
        const glm::vec3 getUp() const;
        const glm::vec3 getRight() const;
        const glm::vec3 getFront() const;

    private:
        int       m_id;
        int       m_parentId;
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        glm::vec3 m_scale;
        bool      m_isNeedToUpdateMatrix;
        bool      m_isScaled;
        glm::mat4 m_modelMatrix;

        void setId(int id);
        void setParentId(int id);
    };
}

#endif