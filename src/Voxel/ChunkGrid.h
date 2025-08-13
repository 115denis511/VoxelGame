#ifndef __VOXEL__CHUNK_GRID_H__
#define __VOXEL__CHUNK_GRID_H__

#include "../stdafx.h"
#include "ChunkGridBounds.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"

namespace engine {
    class ChunkGrid {
    public:
        ChunkGrid(
            ChunkGridBounds& gridBounds, 
            ShaderStorageBuffer<glm::ivec4>& chunkPositionsSSBO,
            VoxelPositionConverter& converter
        );

        void initChunksSSBO();
        void initChunkLocationsInSSBO();
        int getChunkId(int x,  int y, int z);
        VoxelChunk& getChunk(int x, int y, int z);
        VoxelChunk& getChunk(int id);
        VoxelChunk& allocateChunk(int x, int y, int z);
        void freeChunk(int x, int y, int z);
        void freeChunk(int id);
        bool isHaveChunk(int x, int y, int z);
        void syncGpuChunkPositions();
        void resizeToBigger(int distance, std::vector<glm::ivec2>& chunksToCreate);
        void resizeToSmaller(int distance, std::vector<int>& chunksToDelete);
        bool isPositionHasSolidVoxel(const glm::vec3& position);

    private:
        struct GridYSlice {
            int chunk[ChunkGridBounds::CHUNK_MAX_Y_SIZE]{ -1, -1, -1, -1, -1, -1, -1, -1 };  
        };

        ChunkGridBounds& m_gridBounds;
        ShaderStorageBuffer<glm::ivec4>& m_chunkPositionsSSBO;
        VoxelPositionConverter& m_converter;
        GridYSlice m_grid[ChunkGridBounds::CHUNK_MAX_X_Z_SIZE][ChunkGridBounds::CHUNK_MAX_X_Z_SIZE];
        VoxelChunk m_chunks[ChunkGridBounds::CHUNK_COUNT];
        std::stack<size_t> m_freeChunkIndices;
        std::vector<glm::ivec4> m_chunkPositions;
        size_t m_minUpdated{ UINT_MAX }, m_maxUpdated{ 0 };

    };
}

#endif