#ifndef __VOXEL__CHUNK_GRID_H__
#define __VOXEL__CHUNK_GRID_H__

#include "../stdafx.h"
#include "ChunkGridBounds.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"

namespace engine {
    class ChunkGrid {
    public:
        ChunkGrid(VoxelChunk (&chunks)[ChunkGridBounds::CHUNK_COUNT]);

        int getChunkId(int x,  int y, int z);
        VoxelChunk& getChunk(int x, int y, int z);
        void setChunk(int x, int y, int z, int id);
        void resizeToBigger(int distance, ChunkGridBounds& gridBounds, std::vector<glm::ivec2>& chunksToCreate);
        void resizeToSmaller(int distance, ChunkGridBounds& gridBounds, std::vector<int>& chunksToDelete);
        bool isPositionHasSolidVoxel(const glm::vec3& position, const ChunkGridBounds& gridBounds);

    private:
        struct GridYSlice {
            int chunk[ChunkGridBounds::CHUNK_MAX_Y_SIZE]{ -1, -1, -1, -1, -1, -1, -1, -1 };  
        };

        GridYSlice m_grid[ChunkGridBounds::CHUNK_MAX_X_Z_SIZE][ChunkGridBounds::CHUNK_MAX_X_Z_SIZE];
        VoxelChunk* m_chunks;

    };
}

#endif