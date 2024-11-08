#ifndef __VOXEL__VOXEL_VERTEX_H__
#define __VOXEL__VOXEL_VERTEX_H__

#include "../stdafx.h"

namespace engine {
    struct VoxelVertex {
        glm::vec3 position;
        glm::vec3 normal{ 0.0f, 1.0f, 0.0f };
        glm::vec2 texCoord{ 0.f, 0.f };
        int voxelId{ 0 };
        int offsetDirection{ 0 };
    };    

    struct VoxelVertexDataOLD {
        glm::vec3 position;
        GLuint voxelVertexId;
        GLuint offsetDirection;
        // Заполнители места для соответствия размера структуры в разметке std430 структры VertexData в marchingCubes.vert
        GLuint placeholder[3]{ 0, 0, 0 };
    };
    struct VoxelVertexData {
        VoxelVertexData(glm::vec3 tangents[8], glm::vec3 bitangents[8], GLuint voxelVertexId) {
            this->tangents[0] = glm::vec4(tangents[0], 0.f);
            this->tangents[1] = glm::vec4(tangents[1], 0.f);
            this->tangents[2] = glm::vec4(tangents[2], 0.f);
            this->tangents[3] = glm::vec4(tangents[3], 0.f);
            this->tangents[4] = glm::vec4(tangents[4], 0.f);
            this->tangents[5] = glm::vec4(tangents[5], 0.f);
            this->tangents[6] = glm::vec4(tangents[6], 0.f);
            this->tangents[7] = glm::vec4(tangents[7], 0.f);

            this->bitangents[0] = glm::vec4(bitangents[0], 0.f);
            this->bitangents[1] = glm::vec4(bitangents[1], 0.f);
            this->bitangents[2] = glm::vec4(bitangents[2], 0.f);
            this->bitangents[3] = glm::vec4(bitangents[3], 0.f);
            this->bitangents[4] = glm::vec4(bitangents[4], 0.f);
            this->bitangents[5] = glm::vec4(bitangents[5], 0.f);
            this->bitangents[6] = glm::vec4(bitangents[6], 0.f);
            this->bitangents[7] = glm::vec4(bitangents[7], 0.f);
            
            this->voxelVertexId = voxelVertexId;
        }

        glm::vec4 tangents[8];
        glm::vec4 bitangents[8];
        GLuint voxelVertexId;
        GLuint placeholder[3];
    };
}

#endif