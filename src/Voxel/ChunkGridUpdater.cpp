#include "ChunkGridUpdater.h"

engine::ChunkGridUpdater::ChunkGridUpdater(VoxelChunk (&chunks)[ChunkGridBounds::CHUNK_COUNT]) 
    : m_chunks(chunks)
{
    
}

void engine::ChunkGridUpdater::updateChunks(
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

void engine::ChunkGridUpdater::pushToUpdateQueue(size_t index) {
    VoxelChunk& chunk = m_chunks[index];
    if (!chunk.isInUpdateQueue()) {
        chunk.setInUpdateQueueFlag(true);
        m_toUpdateQueue.push(index);
    }
}

void engine::ChunkGridUpdater::pushToUpdateQueueForced(size_t index) {
    m_chunks[index].setInUpdateQueueFlag(true);
    m_toUpdateQueue.push(index);
}

engine::VoxelChunk& engine::ChunkGridUpdater::popFromUpdateQueue() {
    VoxelChunk& chunk = m_chunks[m_toUpdateQueue.top()];
    chunk.setInUpdateQueueFlag(false);
    m_toUpdateQueue.pop();
    return chunk;
}