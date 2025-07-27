#ifndef __VOXEL__MARCHING_CUBES_SOLVER_H__
#define __VOXEL__MARCHING_CUBES_SOLVER_H__

#include "../stdafx.h"
#include "../Render/ShaderStorageBuffer.h"
#include "VoxelChunk.h"
#include "MarchingCubes.h"

namespace engine {
    class MarchingCubesSolver {
    public:
        MarchingCubesSolver();

        void regenerateChunk(MarchingCubes& marchingCubes, VoxelChunk& chunk, ShaderStorageBuffer<glm::ivec2>& globalChunkSSBO);

    private:
        std::vector<glm::ivec2> m_caseData[256];

        void clear();
        uint8_t getCaseIndex(MarchingCubesVoxel voxels[8]);
        glm::ivec2 packData(int x, int y, int z, int offset[6], int textureId[4]);
        
    };
}

#endif