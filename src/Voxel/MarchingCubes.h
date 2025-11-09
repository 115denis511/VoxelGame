#ifndef __VOXEL__MARCHING_CUBES_H__
#define __VOXEL__MARCHING_CUBES_H__

#include "../stdafx.h"
#include "../Render/DrawIndirectCommand.h"
#include "../Render/ShaderStorageBuffer.h"
#include "../Render/UniformManager.h"
#include "MarchingCubesCase.h"
#include "VoxelTriangleData.h"

namespace engine {
    class MarchingCubesManager;

    class MarchingCubes {
        friend MarchingCubesManager;
    public:
        ~MarchingCubes();

        static constexpr float POSITION_OFFSET = 0.5f;
        static constexpr std::array<float, 8> OFFSETS_STRENGTH = { -0.45, -0.375, -0.25, -0.125, 0.0, 0.125, 0.25, 0.375 }; //{ -0.375, -0.25, -0.125, 0.0, 0.125, 0.25, 0.375, 0.5 };
        
        DrawArraysIndirectCommand getCommandTemplate(uint8_t id) { return m_caseDrawCommand[id]; }
        void draw(int drawCount);
        void bindSSBO(GLuint blockId) { m_triangleDataSSBO.bind(blockId); }

    private:
        MarchingCubes();

        DrawArraysIndirectCommand m_caseDrawCommand[256];
        GLuint m_VAO;
        ShaderStorageBuffer<VoxelTriangleData> m_triangleDataSSBO;
        std::vector<VoxelTriangleData> m_trianglesData;

        glm::vec3 roundVector(glm::vec4 vec);
        inline glm::vec3 getNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        inline void addTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, MarchingCubesCase& currentCase);

    };
}

#endif