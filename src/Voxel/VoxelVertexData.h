#ifndef __VOXEL__VOXEL_VERTEX_DATA_H__
#define __VOXEL__VOXEL_VERTEX_DATA_H__

#include "../stdafx.h"

namespace engine {
    struct VoxelVertexData {
        VoxelVertexData(glm::vec3 positions[8], GLuint voxelVertexId, GLuint triangleId) {
            this->positions[0] = glm::vec4(positions[0], 0.f);
            this->positions[1] = glm::vec4(positions[1], 0.f);
            this->positions[2] = glm::vec4(positions[2], 0.f);
            this->positions[3] = glm::vec4(positions[3], 0.f);
            this->positions[4] = glm::vec4(positions[4], 0.f);
            this->positions[5] = glm::vec4(positions[5], 0.f);
            this->positions[6] = glm::vec4(positions[6], 0.f);
            this->positions[7] = glm::vec4(positions[7], 0.f);
            
            this->voxelVertexId = voxelVertexId;
            this->triangleId = triangleId;
        }

        glm::vec4 positions[8];
        GLuint voxelVertexId;
        // Заполнители места для соответствия размера структуры в разметке std430 структры VertexData в marchingCubes.vert
        GLuint triangleId;
        GLuint placeholder[2];
    };
}

#endif