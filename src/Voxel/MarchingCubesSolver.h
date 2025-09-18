#ifndef __VOXEL__MARCHING_CUBES_SOLVER_H__
#define __VOXEL__MARCHING_CUBES_SOLVER_H__

#include "../stdafx.h"
#include "../Render/ShaderStorageBuffer.h"
#include "ChunkGrid.h"
#include "ChunkGridBounds.h"
#include "ChunkVisibilityChecker.h"
#include "MarchingCubes.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"

namespace engine {
    class MarchingCubesSolver {
    public:
        MarchingCubesSolver();

        void regenerateChunk(
            MarchingCubes& marchingCubes, 
            ChunkGrid& grid, 
            glm::ivec3 position, 
            VoxelChunk& chunk, 
            ShaderStorageBuffer<glm::ivec2>& globalChunkSSBO
        );

    private:
        std::vector<glm::ivec2> m_caseData[256];
        int m_cubesCount{ 0 };
        ChunkVisibilityChecker m_checker;

        void accumulateCases(
            MarchingCubes& marchingCubes, 
            ChunkGrid& grid, 
            glm::ivec3 position, 
            VoxelChunk& chunk
        );
        void clear();
        void addMarchingCube(MarchingCubes& marchingCubes, std::array<MarchingCubesVoxel, 8>& voxels, int x, int y, int z);
        uint8_t getCaseIndex(std::array<MarchingCubesVoxel, 8>& voxels);
        glm::ivec2 packData(int x, int y, int z, int offset[6], int textureId[4]);
        
    };
}

#endif