#include "ChunkGridChanger.h"

engine::ChunkGridChanger::ChunkGridChanger(ChunkGrid& grid) 
    : m_grid(grid)
{
    
}

void engine::ChunkGridChanger::generateChunks(
    ChunkGridBounds& gridBounds, 
    VoxelPositionConverter& converter,
    bool usingGlobalChunkSSBO, 
    size_t maxSlices
) {
    while (!m_toGenerateQueue.empty() && maxSlices > 0) {
        glm::ivec2 pos = m_toGenerateQueue.back();
        glm::ivec2 localPos = converter.worldChunkToLocalChunkPosition(pos.x, pos.y, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
        m_toGenerateQueue.pop_back();

        if (!gridBounds.isWorldChunkInbounds(pos.x, 0, pos.y)) continue; 
        if (m_grid.isHaveChunk(localPos.x, 0, localPos.y)) continue;

        for (int y = 0; y < gridBounds.CHUNK_MAX_Y_SIZE; y++) {
            VoxelChunk& chunk = m_grid.allocateChunk(localPos.x, y, localPos.y);
            int id = m_grid.getChunkId(localPos.x, y, localPos.y);

            chunk.clear();
            chunk.updateVisibilityStatesForEmptyChunk();
            chunk.clearDrawCommands();

            pushToUpdateQueueForced(id);

            // Временный код генерации мира
            if (y == 0) {
                for (size_t x = 0; x < 32; x++) {
                    for (size_t z = 0; z < 32; z++){
                        chunk.setVoxel(x,1,z, 0);
                    }
                }

                if (pos.x <= -1 && std::abs(pos.x) >= std::abs(pos.y)) {
                    for (size_t y = 0; y < 32; y++) {
                        for (size_t z = 0; z < 32; z++){
                            chunk.setVoxel(0,y,z, 1);
                            chunk.setVoxel(31,y,z, 1);
                            chunk.setVoxel(z,y,0, 1);
                            chunk.setVoxel(z,y,31, 1);
                            chunk.setVoxel(z,0,y, 1);
                            chunk.setVoxel(z,31,y, 1);
                        }
                    }
                }
            }
        }

        maxSlices--;
    }

    if (usingGlobalChunkSSBO) { m_grid.syncGpuChunkPositions(); }
}

void engine::ChunkGridChanger::updateChunks(
    MarchingCubes &marchingCubes,
    ShaderStorageBuffer<glm::ivec2> &globalChunkSSBO,
    size_t maxCount
) {
    for (size_t i = 0; i < maxCount && !m_toUpdateQueue.empty(); i++) {
        VoxelChunk& chunk = popFromUpdateQueue();
        if (!chunk.isInUse()) { continue; }

        m_solver.regenerateChunk(marchingCubes, chunk, globalChunkSSBO);
    }
}

void engine::ChunkGridChanger::resizeGrid(
    ChunkGridBounds& gridBounds, 
    ShaderStorageBuffer<glm::ivec4>& chunkPositionsSSBO,
    VoxelPositionConverter& converter,
    bool usingGlobalChunkSSBO,
    int size
) {
    if (size % 2 == 1) size -= 1;
    if (size > gridBounds.CHUNK_MAX_X_Z_SIZE) size = gridBounds.CHUNK_MAX_X_Z_SIZE;
    int usedChunkDistance = static_cast<int>(gridBounds.usedChunkGridWidth);
    if (usedChunkDistance == size) return;

    if (size < usedChunkDistance) {
        std::vector<int> chunksToDelete;
        m_grid.resizeToSmaller(size, chunksToDelete); 

        for (int id : chunksToDelete) {
            m_grid.freeChunk(id);
        }
    }
    else {
        m_toGenerateQueue.clear();
        m_grid.resizeToBigger(size, m_toGenerateQueue);
    }
}

void engine::ChunkGridChanger::pushToUpdateQueue(size_t index) {
    VoxelChunk& chunk = m_grid.getChunk(index);
    if (!chunk.isInUpdateQueue()) {
        chunk.setInUpdateQueueFlag(true);
        m_toUpdateQueue.push(index);
    }
}

void engine::ChunkGridChanger::pushToUpdateQueueForced(size_t index) {
    m_grid.getChunk(index).setInUpdateQueueFlag(true);
    m_toUpdateQueue.push(index);
}

engine::VoxelChunk& engine::ChunkGridChanger::popFromUpdateQueue() {
    VoxelChunk& chunk = m_grid.getChunk(m_toUpdateQueue.top());
    chunk.setInUpdateQueueFlag(false);
    m_toUpdateQueue.pop();
    return chunk;
}