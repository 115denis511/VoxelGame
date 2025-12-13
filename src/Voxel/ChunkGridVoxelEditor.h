#ifndef __VOXEL__CHUNK_GRID_VOXEL_EDITOR_H__
#define __VOXEL__CHUNK_GRID_VOXEL_EDITOR_H__

#include "../stdafx.h"
#include "ChunkGrid.h"
#include "ChunkGridBounds.h"
#include "ChunkGridChanger.h"
#include "Voxel.h"

namespace engine {
    class ChunkGridVoxelEditor {
    public:
        ChunkGridVoxelEditor(ChunkGrid& grid, ChunkGridBounds& bounds, ChunkGridChanger& changer, VoxelPositionConverter& converter) 
            : m_grid(grid), m_gridBounds(bounds), m_gridChanger(changer), m_converter(converter) {}

        void setSolidVoxel(const glm::vec3& position, uint8_t id, uint8_t size = 3);
        void setLiquidVoxel(const glm::vec3& position, uint8_t id, uint8_t size = 3);

    private:
        ChunkGrid& m_grid;
        ChunkGridBounds& m_gridBounds;
        ChunkGridChanger& m_gridChanger;
        VoxelPositionConverter& m_converter;

        void syncChunkBorders(const glm::ivec3& chunkWorldPos, const glm::ivec2& chunkLocalPos, const glm::ivec3& localVoxelPos, const Voxel& voxel);
        void syncLeftChunk(const glm::ivec3& chunkWorldPos, const glm::ivec2& chunkLocalPos, const Voxel& voxel, int y, int z);
        void syncBottomChunk(const glm::ivec3& chunkWorldPos, const glm::ivec2& chunkLocalPos, const Voxel& voxel, int x, int z);
        void syncBackChunk(const glm::ivec3& chunkWorldPos, const glm::ivec2& chunkLocalPos, const Voxel& voxel, int x, int y);
        void syncLeftBottomChunk(const glm::ivec3& chunkWorldPos, const glm::ivec2& chunkLocalPos, const Voxel& voxel, int z);
        void syncLeftBackChunk(const glm::ivec3& chunkWorldPos, const glm::ivec2& chunkLocalPos, const Voxel& voxel, int y);
        void syncBottomBackChunk(const glm::ivec3& chunkWorldPos, const glm::ivec2& chunkLocalPos, const Voxel& voxel, int x);
        void syncLeftBottomBackChunk(const glm::ivec3& chunkWorldPos, const glm::ivec2& chunkLocalPos, const Voxel& voxel);
    };
}

#endif