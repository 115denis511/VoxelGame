#ifndef __VOXEL__MARCHING_CUBES_H__
#define __VOXEL__MARCHING_CUBES_H__

#include "../stdafx.h"
#include "../Render/DrawIndirectCommand.h"
#include "MarchingCubesCase.h"
#include "VoxelVertex.h"

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
        std::vector<VoxelVertex> m_vertices;
        GLuint m_VAO, m_VBO;
        GLuint m_dbg_texture;

        glm::vec3 roundVector(glm::vec4 vec);
        glm::vec3 getNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        inline void addTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, 
                                MarchingCubesCase& currentCase,
                                const glm::vec2& texCoord0, const glm::vec2& texCoord1, const glm::vec2& texCoord2);

        glm::ivec2 debugPackData(int x, int y, int z);

    };
}

#endif