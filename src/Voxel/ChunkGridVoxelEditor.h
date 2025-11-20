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

        void setVoxel(const glm::vec3& position, uint8_t id, uint8_t size = 3, Voxel::Type type = Voxel::Type::SOLID);

    private:
        ChunkGrid& m_grid;
        ChunkGridBounds& m_gridBounds;
        ChunkGridChanger& m_gridChanger;
        VoxelPositionConverter& m_converter;

    };
}

#endif