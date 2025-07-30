#ifndef __VOXEL__CHUNK_GRID_UPDATER_H__
#define __VOXEL__CHUNK_GRID_UPDATER_H__

#include "../stdafx.h"
#include "../Render/ShaderStorageBuffer.h"
#include "ChunkGridBounds.h"
#include "MarchingCubes.h"
#include "MarchingCubesSolver.h"
#include "VoxelChunk.h"

namespace engine {
    class ChunkGridUpdater{
    public:
        ChunkGridUpdater(VoxelChunk (&chunks)[ChunkGridBounds::CHUNK_COUNT]);

        //void generateChunks(size_t maxUpdates = 8);
        void updateChunks(
            MarchingCubes& marchingCubes, 
            ShaderStorageBuffer<glm::ivec2>& globalChunkSSBO, 
            size_t maxUpdates = 8
        );
        void pushToUpdateQueue(size_t index);
        void pushToUpdateQueueForced(size_t index);
        VoxelChunk& popFromUpdateQueue();

    private:
        VoxelChunk* m_chunks;
        std::stack<size_t> m_toUpdateQueue;
        //std::vector<glm::ivec2> m_toGenerateQueue;
        MarchingCubesSolver m_solver;
    };
}

#endif