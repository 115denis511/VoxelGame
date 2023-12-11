#ifndef __UTILITES__ASSIMP_CONVERTER_H__
#define __UTILITES__ASSIMP_CONVERTER_H__

#include "../stdafx.h"

namespace utilites {
    namespace assimp_convert_to {
        inline glm::vec3 glm(aiVector3D& vector) {
            return glm::vec3(vector.x, vector.y, vector.z);
        }

        inline glm::quat glm(aiQuaternion &aiQuat) {
            return glm::quat(aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z);
        }

        inline glm::mat4 glm(aiMatrix4x4 &matrix) {
            glm::mat4 matGLM;
            matGLM[0][0] = matrix.a1; matGLM[1][0] = matrix.a2; matGLM[2][0] = matrix.a3; matGLM[3][0] = matrix.a4;
            matGLM[0][1] = matrix.b1; matGLM[1][1] = matrix.b2; matGLM[2][1] = matrix.b3; matGLM[3][1] = matrix.b4;
            matGLM[0][2] = matrix.c1; matGLM[1][2] = matrix.c2; matGLM[2][2] = matrix.c3; matGLM[3][2] = matrix.c4;
            matGLM[0][3] = matrix.d1; matGLM[1][3] = matrix.d2; matGLM[2][3] = matrix.d3; matGLM[3][3] = matrix.d4;
            return matGLM;
        }
    }
}

#endif