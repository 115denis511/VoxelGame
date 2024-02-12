#ifndef __SCENE__CAMERA_H__
#define __SCENE__CAMERA_H__

#include "../stdafx.h"

namespace engine {
    enum class CameraMode {
        FIRST_PERSON_VIEW,
        THIRD_PERSON_VIEW
    };

    struct CameraVars {
        glm::mat4 lookAt;
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraUp;
        glm::vec3 cameraRight; 
        static constexpr glm::vec3 worldUp{0.f, 1.f, 0.f};
    };

    class Camera {
    public:
        Camera();

        CameraVars  getVars();
        glm::mat4   getLookAt();
        void        updateLookAt();
        void        setMode(CameraMode mode);
        void        setYaw(GLfloat yaw);
        void        setPitch(GLfloat pitch);
        void        setDistance(GLfloat distance);
        const glm::vec3 getPosition() const;
        const glm::vec3 getRight() const;
        const glm::vec3 getFront() const;
        const glm::vec3 getUp() const;
        void        moveRotation(GLfloat yaw, GLfloat pitch);
        void        moveRotation(const glm::vec2& offset);
        void        movePosition(const glm::vec3& offset);

    private:
        CameraMode m_cameraMode;
        /* 
            Вспомогательная переменная для инвертирования знака некоторых значений умножением на -1 при виде от третьего лица.
            При виде от первого лица равна 1, что оставляет значения без изменений.
            Данный хак применяется для избежания сравнений режима камеры при вычислениях в большенстве функций.
        */
        GLfloat m_modeMultiplier;
        static constexpr glm::vec3 g_worldUp{0.f, 1.f, 0.f};
        glm::mat4 m_lookAt;
        glm::vec3 m_cameraPosition;
        glm::vec3 m_cameraTarget;
        glm::vec3 m_cameraUp;
        glm::vec3 m_cameraRight;
        GLfloat m_yaw;
        GLfloat m_pitch;
        GLfloat m_distance;

    };
}

#endif