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

        void generateChunks(bool usingGlobalChunkSSBO, size_t maxSlices);
        void updateChunks(MarchingCubes& marchingCubes, ShaderStorageBuffer<glm::ivec2>& globalChunkSSBO, size_t maxUpdates = 8);
        void resizeGrid(ShaderStorageBuffer<glm::ivec4>& chunkPositionsSSBO, bool usingGlobalChunkSSBO, int size);
        void pushToUpdateQueue(size_t index);
        void pushToUpdateQueueForced(size_t index);
        size_t popFromUpdateQueue();

    private:
        static constexpr int MAX_QUEUE_CAHNGES_WITHOUT_SORTING = 32;

        ChunkGrid& m_grid;
        std::vector<size_t> m_toUpdateQueue;
        int m_updateQueueChanges{ 0 };
        std::vector<glm::ivec2> m_toGenerateQueue;
        int m_generateQueueChanges{ 0 };
        MarchingCubesSolver m_solver;

        void refineChunkBorders(ChunkGridBounds& gridBounds, glm::ivec2 localPos);
        void sortUpdateQueue();
    };
}

#endif