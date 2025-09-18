#ifndef __VOXEL__CHUNK_VISIBILITY_CHECKER_H__
#define __VOXEL__CHUNK_VISIBILITY_CHECKER_H__

#include "../stdafx.h"
#include "ChunkGrid.h"
#include "ChunkVisibilityState.h"
#include "VoxelChunk.h"

namespace engine {
    class ChunkVisibilityChecker {
    public:
        ChunkVisibilityChecker() {}

        void updateVisibilityStates(ChunkGrid& grid, VoxelChunk& chunk, glm::ivec3 worldPosition);

    private:
        enum class VisibilityCheckState : uint8_t { NOT_CHECKED = 0, CHECKED };

        static constexpr int VOXEL_CHUNK_SIZE = (int)VoxelChunk::VOXEL_CHUNK_SIZE;
        static constexpr int MARCHING_CUBE_OVERLAPING_BORDER = 1;
        static constexpr int STATE_GRID_SIZE = VOXEL_CHUNK_SIZE + MARCHING_CUBE_OVERLAPING_BORDER;

        VisibilityCheckState m_state [STATE_GRID_SIZE][STATE_GRID_SIZE][STATE_GRID_SIZE];
        std::stack<glm::ivec3> m_stack;

        void clearStates();
        int floodFill(int x, int y, int z, VoxelChunk& chunk, ChunkVisibilityState& visabilityState);
        void floodFillScanNext(int lx, int rx, int y, int z, VoxelChunk& chunk);
        bool isVoxelEmptyAndNotChecked(int x, int y, int z, VoxelChunk& chunk) {
            return !chunk.isVoxelSolid(x, y, z) && m_state[x][y][z] == VisibilityCheckState::NOT_CHECKED;
        };
        
    };
}

#endif