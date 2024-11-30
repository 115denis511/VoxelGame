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

    struct VoxelTriangleVariant {
        glm::vec4 pos_TBN_tex[6];

        VoxelTriangleVariant() {}
        VoxelTriangleVariant(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
                             const glm::vec3& tangent, const glm::vec3& bitangent, const glm::vec3& normal,
                             const glm::vec2& texA, const glm::vec2& texB, const glm::vec2& texC) {
            pos_TBN_tex[0] = glm::vec4(posA, texA.x);
            pos_TBN_tex[1] = glm::vec4(posB, texA.y);
            pos_TBN_tex[2] = glm::vec4(posC, texB.x);
            pos_TBN_tex[3] = glm::vec4(tangent, texB.y);
            pos_TBN_tex[4] = glm::vec4(bitangent, texC.x);
            pos_TBN_tex[5] = glm::vec4(normal, texC.y);
        }

        const glm::vec3 getPositionA() const { return glm::vec3(pos_TBN_tex[0].x , pos_TBN_tex[0].y, pos_TBN_tex[0].z); }
        const glm::vec3 getPositionB() const { return glm::vec3(pos_TBN_tex[1].x , pos_TBN_tex[1].y, pos_TBN_tex[1].z); }
        const glm::vec3 getPositionC() const { return glm::vec3(pos_TBN_tex[2].x , pos_TBN_tex[2].y, pos_TBN_tex[2].z); }
        const glm::vec3 getTangent() const   { return glm::vec3(pos_TBN_tex[3].x , pos_TBN_tex[3].y, pos_TBN_tex[3].z); }
        const glm::vec3 getBitangent() const { return glm::vec3(pos_TBN_tex[4].x , pos_TBN_tex[4].y, pos_TBN_tex[4].z); }
        const glm::vec3 getNormal() const    { return glm::vec3(pos_TBN_tex[5].x , pos_TBN_tex[5].y, pos_TBN_tex[5].z); }
        const glm::vec2 getTexCoordA() const { return glm::vec2(pos_TBN_tex[0].w, pos_TBN_tex[1].w); }
        const glm::vec2 getTexCoordB() const { return glm::vec2(pos_TBN_tex[2].w, pos_TBN_tex[3].w); }
        const glm::vec2 getTexCoordC() const { return glm::vec2(pos_TBN_tex[4].w, pos_TBN_tex[5].w); }
    };
    struct VoxelTriangleData {
        VoxelTriangleVariant pos_TBN_tex[8 * 8 * 8];
        glm::uvec4 figureIds;

        VoxelTriangleData() {}
    };
}

#endif