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
    class ChunkBuilder {
    public:
        ChunkBuilder();

        void regenerateChunk(
            MarchingCubes& marchingCubes, 
            ChunkGrid& grid,
            VoxelChunk& chunk, 
            ShaderStorageBuffer<GLuint>& globalChunkSSBO,
            ShaderStorageBuffer<Voxel>& globalChunkGridsSSBO
        );

    private:
        static constexpr int CASE_COUNT = 256;

        std::vector<GLuint> m_solidCaseData[CASE_COUNT];
        std::vector<GLuint> m_liquidCaseData[CASE_COUNT];
        int m_solidCubesCount{ 0 };
        int m_liquidCubesCount{ 0 };
        ChunkVisibilityChecker m_checker;

        void accumulateCases(MarchingCubes& marchingCubes, ChunkGrid& grid,VoxelChunk& chunk);
        void clear();
        void addMarchingCube(MarchingCubes& marchingCubes, std::array<Voxel, 8>& voxels, int x, int y, int z);
        uint8_t getSolidCaseIndex(std::array<Voxel, 8>& voxels);
        uint8_t getLiquidCaseIndex(std::array<Voxel, 8>& voxels);
        GLuint packData(int x, int y, int z);
        
    };
}

#endif