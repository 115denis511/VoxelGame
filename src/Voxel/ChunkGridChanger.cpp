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