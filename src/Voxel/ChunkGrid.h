#ifndef __VOXEL__CHUNK_GRID_H__
#define __VOXEL__CHUNK_GRID_H__

#include "../stdafx.h"
#include "VoxelChunk.h"

namespace engine {
    class ChunkGrid {
    public:
        ChunkGrid();

        static constexpr int CHUNK_MAX_X_Z_SIZE = 24;
        static constexpr int CHUNK_MAX_Y_SIZE = 8;

        int getChunkId(int x,  int y, int z);
        unsigned int getUsedChunkDistance() { return m_usedChunkDistance; }
        void setChunk(int x, int y, int z, int id);
        void resizeToBigger(unsigned int distance, std::vector<glm::ivec2>& chunksToCreate);
        void resizeToSmaller(unsigned int distance, std::vector<int>& chunksToDelete);

    private:
        struct GridYSlice {
            int chunk[CHUNK_MAX_Y_SIZE]{ -1, -1, -1, -1, -1, -1, -1, -1 };  
        };

        GridYSlice m_grid[CHUNK_MAX_X_Z_SIZE][CHUNK_MAX_X_Z_SIZE];
        unsigned int m_usedChunkDistance{ 0 };

    };
}

#endif