#ifndef __VOXEL__CHUNK_VISIBILITY_CHECKER_H__
#define __VOXEL__CHUNK_VISIBILITY_CHECKER_H__

#include "../stdafx.h"
#include "BinaryGrid.h"
#include "ChunkGrid.h"
#include "ChunkVisibilityState.h"
#include "VoxelChunk.h"

namespace engine {
    class ChunkVisibilityChecker {
    public:
        ChunkVisibilityChecker() {}

        void updateVisibilityStates(ChunkGrid& grid, VoxelChunk& chunk);

    private:
        static constexpr int STATE_GRID_SIZE = VoxelChunk::VOXELS_PER_AXIS;

        BinaryGrid<uint32_t> m_state;
        std::stack<glm::ivec2> m_stack;

        void clearStates();
        int floodFill(int x, int y, int z, VoxelChunk& chunk, ChunkVisibilityState& visabilityState);
        uint32_t expandRow(uint32_t orig, uint32_t empty);
        int checkNeighbourRow(int y, int z, uint32_t expand, const BinaryGrid<uint32_t>& grid, ChunkVisibilityState& visabilityState);
        int checkIsOnBorder(uint32_t row, glm::ivec2 pos, ChunkVisibilityState& visabilityState);
        bool isVoxelEmptyAndNotChecked(int x, int y, int z, VoxelChunk& chunk) {
            return !chunk.isVoxelSolid(x, y, z) && !m_state.get(x, y, z);
        };
        
    };
}

#endif