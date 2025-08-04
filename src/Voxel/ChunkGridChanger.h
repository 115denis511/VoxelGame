#ifndef __VOXEL__CHUNK_GRID_CHANGER_H__
#define __VOXEL__CHUNK_GRID_CHANGER_H__

#include "../stdafx.h"
#include "../Render/ShaderStorageBuffer.h"
#include "ChunkGrid.h"
#include "ChunkGridBounds.h"
#include "MarchingCubes.h"
#include "MarchingCubesSolver.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"

namespace engine {
    class ChunkGridChanger{
    public:
        ChunkGridChanger(ChunkGrid& grid);

        //void generateChunks(size_t maxUpdates = 8);
        void updateChunks(
            MarchingCubes& marchingCubes, 
            ShaderStorageBuffer<glm::ivec2>& globalChunkSSBO, 
            size_t maxUpdates = 8
        );
        void resizeGrid(
            ChunkGridBounds& gridBounds, 
            ShaderStorageBuffer<glm::ivec4>& chunkPositionsSSBO,
            VoxelPositionConverter& converter,
            bool usingGlobalChunkSSBO,
            int size
        );
        void pushToUpdateQueue(size_t index);
        void pushToUpdateQueueForced(size_t index);
        VoxelChunk& popFromUpdateQueue();

    private:
        ChunkGrid& m_grid;
        std::stack<size_t> m_toUpdateQueue;
        std::vector<glm::ivec2> m_toGenerateQueue;
        MarchingCubesSolver m_solver;
    };
}

#endif