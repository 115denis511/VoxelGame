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

    struct VoxelVertexData {
        glm::vec3 position;
        GLuint voxelVertexId;
        GLuint offsetDirection;
        // Заполнители места для соответствия размера структуры в разметке std430 структры VertexData в marchingCubes.vert
        GLuint placeholder[3]{ 0, 0, 0 };
    };
}

#endif