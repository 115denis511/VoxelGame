#include "ChunkGridChanger.h"

engine::ChunkGridChanger::ChunkGridChanger(ChunkGrid& grid) 
    : m_grid(grid)
{
    
}

void engine::ChunkGridChanger::updateChunks(
    MarchingCubes& marchingCubes,
    ShaderStorageBuffer<glm::ivec2>& globalChunkSSBO,
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

        for (glm::ivec2& pos : m_toGenerateQueue) {
            for (int y = 0; y < gridBounds.CHUNK_MAX_Y_SIZE; y++) {
                VoxelChunk& chunk = m_grid.allocateChunk(pos.x, y, pos.y);
                int id = m_grid.getChunkId(pos.x, y, pos.y);

                // TODO: Переместить код генерации мира в updateChunks.
                // Затем убрать очистку чанка отсюда(убрать следующую строку и раскоментировать последующую).
                chunk.clear();

                // Временная "прозрачность" видимости чанка.
                // Пока чанк находится в очереди на перегенерацию, оставшиеся старые состояния видимости
                // могут блокировать видимость через пустой чанк.
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

                    if (pos.x <= 0 && std::abs(pos.x) <= std::abs(y)) {
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
        }

        if (usingGlobalChunkSSBO) { m_grid.syncGpuChunkPositions(); }
        
        m_toGenerateQueue.clear();
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