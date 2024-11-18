#ifndef __VOXEL__MARCHING_CUBES_H__
#define __VOXEL__MARCHING_CUBES_H__

#include "../stdafx.h"
#include "../Render/DrawIndirectCommand.h"
#include "MarchingCubesCase.h"
#include "VoxelVertexData.h"
#include "../Render/UniformManager.h"

namespace engine {
    class MarchingCubesManager;

    class MarchingCubes {
        friend MarchingCubesManager;
    public:
        ~MarchingCubes();
        
        MarchingCubesVertecesIds getVertecesIds(uint8_t id) { return m_caseVertecesIds[id]; }
        DrawArraysIndirectCommand getCommandTemplate(uint8_t id) { return m_caseDrawCommand[id]; }
        void draw(int drawCount);

    private:
        MarchingCubes();

        DrawArraysIndirectCommand m_caseDrawCommand[256];
        MarchingCubesVertecesIds m_caseVertecesIds[256];
        GLuint m_VAO, m_verticesDataSSBO;
        std::vector<VoxelVertexData> m_verticesData;

        glm::vec3 roundVector(glm::vec4 vec);
        inline glm::vec3 getNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        inline void addTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, MarchingCubesCase& currentCase, GLuint triangleId);

    };
}

#endif