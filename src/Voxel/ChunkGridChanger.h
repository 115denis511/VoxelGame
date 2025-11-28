#ifndef __VOXEL__CHUNK_GRID_CHANGER_H__
#define __VOXEL__CHUNK_GRID_CHANGER_H__

#include "../stdafx.h"
#include "../Render/ShaderStorageBuffer.h"
#include "ChunkBuilder.h"
#include "ChunkGrid.h"
#include "ChunkGridBounds.h"
#include "IChunkLoader.h"
#include "MarchingCubes.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"

namespace engine {
    class ChunkGridChanger{
    public:
        ChunkGridChanger(ChunkGrid& grid);
        ~ChunkGridChanger();

        void generateChunks(bool usingGlobalChunkSSBO, size_t maxSlices);
        void updateChunks(
            MarchingCubes& marchingCubes, 
            ShaderStorageBuffer<GLuint>& globalChunkSSBO, 
            ShaderStorageBuffer<Voxel>& globalChunkGridsSSBO, 
            size_t maxUpdates = 8
        );
        void resizeGrid(ShaderStorageBuffer<glm::ivec4>& chunkPositionsSSBO, bool usingGlobalChunkSSBO, int size);
        void pushToUpdateQueue(size_t index);
        void pushToUpdateQueueForced(size_t index);
        size_t popFromUpdateQueue();
        void setChunkLoader(IChunkLoader* newLoader);

    private:
        ChunkGrid& m_grid;
        std::vector<size_t> m_toUpdateQueue;
        bool m_isToUpdateQueueNeedSort{ false };
        std::vector<glm::ivec2> m_toGenerateQueue;
        ChunkBuilder m_chunkBuilder;
        IChunkLoader* m_loader{ new NullChunkLoader() };

        void refineChunkBorders(ChunkGridBounds& gridBounds, glm::ivec2 localPos);
        void sortToUpdateQueue();
        void sortToGenerateQueue();
    };
}

#endif