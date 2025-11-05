#include "ChunkGridChanger.h"

engine::ChunkGridChanger::ChunkGridChanger(ChunkGrid& grid) 
    : m_grid(grid)
{
    
}

engine::ChunkGridChanger::~ChunkGridChanger() {
    if (m_loader != nullptr) delete m_loader;
}

void engine::ChunkGridChanger::generateChunks(bool usingGlobalChunkSSBO, size_t maxSlices) {
    ChunkGridBounds& gridBounds = m_grid.getGridBounds();
    VoxelPositionConverter& converter = m_grid.getPositionConverter();

    while (!m_toGenerateQueue.empty() && maxSlices > 0) {
        glm::ivec2 pos = m_toGenerateQueue.back();
        glm::ivec2 localPos = converter.worldChunkToLocalChunkPosition(pos.x, pos.y, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
        m_toGenerateQueue.pop_back();

        if (!gridBounds.isWorldChunkInbounds(pos.x, 0, pos.y)) continue; 
        if (m_grid.isHaveChunk(localPos.x, 0, localPos.y)) continue;

        std::array<VoxelChunkBase*, 8> chunkSlice;

        for (int y = 0; y < gridBounds.CHUNK_MAX_Y_SIZE; y++) {
            VoxelChunk& chunk = m_grid.allocateChunk(localPos.x, y, localPos.y);
            int id = m_grid.getChunkId(localPos.x, y, localPos.y);
            chunkSlice[y] = &chunk;

            chunk.clear();
            chunk.clearVisibilityStatesForEmptyChunk();
            chunk.clearDrawCommands();

            pushToUpdateQueueForced(id);
        }
        
        m_loader->load(pos, chunkSlice);
        refineChunkBorders(gridBounds, localPos);

        maxSlices--;
    }

    if (usingGlobalChunkSSBO) { m_grid.syncGpuChunkPositions(); }
}

void engine::ChunkGridChanger::updateChunks(
    MarchingCubes &marchingCubes,
    ShaderStorageBuffer<glm::ivec2>& globalChunkSSBO,
    ShaderStorageBuffer<Voxel>& globalChunkGridsSSBO,
    size_t maxCount
) {
    if (m_isToUpdateQueueNeedSort) {
        sortToUpdateQueue();
        m_isToUpdateQueueNeedSort = false;
    }

    for (size_t i = 0; i < maxCount && !m_toUpdateQueue.empty(); i++) {
        size_t id = popFromUpdateQueue();
        VoxelChunk& chunk = m_grid.getChunk(id);
        if (!chunk.isInUse()) { continue; }

        m_solver.regenerateChunk(marchingCubes, m_grid, chunk, globalChunkSSBO, globalChunkGridsSSBO);
    }
}

void engine::ChunkGridChanger::resizeGrid(ShaderStorageBuffer<glm::ivec4>& chunkPositionsSSBO, bool usingGlobalChunkSSBO, int size) {
    ChunkGridBounds& gridBounds = m_grid.getGridBounds();

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

    sortToGenerateQueue(); // TODO: После сортировки багуются углы чанков
}

void engine::ChunkGridChanger::pushToUpdateQueue(size_t index) {
    VoxelChunk& chunk = m_grid.getChunk(index);
    if (!chunk.isInUpdateQueue()) {
        chunk.setInUpdateQueueFlag(true);
        m_toUpdateQueue.push_back(index);
    }

    m_isToUpdateQueueNeedSort = true;
}

void engine::ChunkGridChanger::pushToUpdateQueueForced(size_t index) {
    m_grid.getChunk(index).setInUpdateQueueFlag(true);
    m_toUpdateQueue.push_back(index);

    m_isToUpdateQueueNeedSort = true;
}

size_t engine::ChunkGridChanger::popFromUpdateQueue() {
    VoxelChunk& chunk = m_grid.getChunk(m_toUpdateQueue.back());
    chunk.setInUpdateQueueFlag(false);

    size_t id = m_toUpdateQueue.back();
    m_toUpdateQueue.pop_back();
    return id;
}

void engine::ChunkGridChanger::setChunkLoader(IChunkLoader* newLoader) {
    if (m_loader != nullptr) delete m_loader;
    m_loader = newLoader;
}

void engine::ChunkGridChanger::refineChunkBorders(ChunkGridBounds& gridBounds, glm::ivec2 localPos) {
    VoxelChunk* chunks[ChunkGridBounds::CHUNK_MAX_Y_SIZE];
    for (int y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
        chunks[y] = &m_grid.getChunk(localPos.x, y, localPos.y);
    }
    
    glm::ivec2 current = glm::ivec2(localPos.x + 1, localPos.y); // X+
    if (gridBounds.isChunkXZInbounds(current.x, current.y) && m_grid.isHaveChunk(current.x, 0, current.y)) {
        for (int y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
            int chunkId = m_grid.getChunkId(current.x, y, current.y);
            
            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            for (int vy = 0; vy < VoxelChunk::VOXEL_CHUNK_SIZE; vy++) {
                for (int vz = 0; vz < VoxelChunk::VOXEL_CHUNK_SIZE; vz++) {
                    Voxel voxel = heighbour.getVoxel(0, vy, vz);
                    chunks[y]->setVoxel(32, vy, vz, voxel);
                }
            }
        }
    }

    current = glm::ivec2(localPos.x, localPos.y + 1); // Z+
    if (gridBounds.isChunkXZInbounds(current.x, current.y) && m_grid.isHaveChunk(current.x, 0, current.y)) {
        for (int y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
            int chunkId = m_grid.getChunkId(current.x, y, current.y);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            for (int vy = 0; vy < VoxelChunk::VOXEL_CHUNK_SIZE; vy++) {
                for (int vx = 0; vx < VoxelChunk::VOXEL_CHUNK_SIZE; vx++) {
                    Voxel voxel = heighbour.getVoxel(vx, vy, 0);
                    chunks[y]->setVoxel(vx, vy, 32, voxel);
                }
            }
        }
    }

    current = glm::ivec2(localPos.x + 1, localPos.y + 1); // X+ Z+
    if (gridBounds.isChunkXZInbounds(current.x, current.y) && m_grid.isHaveChunk(current.x, 0, current.y)) {
        for (int y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
            int chunkId = m_grid.getChunkId(current.x, y, current.y);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            for (int vy = 0; vy < VoxelChunk::VOXEL_CHUNK_SIZE; vy++) {
                Voxel voxel = heighbour.getVoxel(0, vy, 0);
                chunks[y]->setVoxel(32, vy, 32, voxel);
            }
        }
    }

    for (int y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE - 1; y++) { // Y-
        for (int x = 0; x < VoxelChunk::GRID_SIZE; x++) {
            for (int z = 0; z < VoxelChunk::GRID_SIZE; z++) {
                Voxel voxel = chunks[y + 1]->getVoxel(x, 0, z);
                chunks[y]->setVoxel(x, 32, z, voxel);
            }
        }
    }

    // Для того, чтобы обойти дополнительную корректировку границ соседних чанков со сгенерированным чанком на один слой выше
    // исправление соседних чанков должно происходить после исправления границ сгенерированных чанков.
    // После корректировки Y-границы между сгенерированными чанками нижний чанк уже хранит копию состояния нижней границы верхнего чанка, 
    // что позволяет соседнему чанку сразу получить состояние границы обоих сгенерированных чанков из нижнего.

    current = glm::ivec2(localPos.x - 1, localPos.y); // X-
    if (gridBounds.isChunkXZInbounds(current.x, current.y) && m_grid.isHaveChunk(current.x, 0, current.y)) {
        for (int y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
            int chunkId = m_grid.getChunkId(current.x, y, current.y);
            pushToUpdateQueue(chunkId);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            for (int vy = 0; vy < VoxelChunk::GRID_SIZE; vy++) {
                for (int vz = 0; vz < VoxelChunk::GRID_SIZE; vz++) {
                    Voxel voxel = chunks[y]->getVoxel(0, vy, vz);
                    heighbour.setVoxel(32, vy, vz, voxel);
                }
            }
        }
    }
    
    current = glm::ivec2(localPos.x, localPos.y - 1); // Z-
    if (gridBounds.isChunkXZInbounds(current.x, current.y) && m_grid.isHaveChunk(current.x, 0, current.y)) {
        for (int y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
            int chunkId = m_grid.getChunkId(current.x, y, current.y);
            pushToUpdateQueue(chunkId);
            
            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            for (int vy = 0; vy < VoxelChunk::GRID_SIZE; vy++) {
                for (int vx = 0; vx < VoxelChunk::GRID_SIZE; vx++) {
                    Voxel voxel = chunks[y]->getVoxel(vx, vy, 0);
                    heighbour.setVoxel(vx, vy, 32, voxel);
                }
            }
        }
    }
    
    current = glm::ivec2(localPos.x - 1, localPos.y - 1); // X- Z-
    if (gridBounds.isChunkXZInbounds(current.x, current.y) && m_grid.isHaveChunk(current.x, 0, current.y)) {
        for (int y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
            int chunkId = m_grid.getChunkId(current.x, y, current.y);
            pushToUpdateQueue(chunkId);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            for (int vy = 0; vy < VoxelChunk::GRID_SIZE; vy++) {
                Voxel voxel = chunks[y]->getVoxel(0, vy, 0);
                heighbour.setVoxel(32, vy, 32, voxel);
            }
        }
    }
}

void engine::ChunkGridChanger::sortToUpdateQueue() {
    ChunkGridBounds& gridBounds = m_grid.getGridBounds();
    
    glm::ivec2 gridCenter = gridBounds.currentCenterChunk;
    sort(m_toUpdateQueue.begin(), m_toUpdateQueue.end(), [&](const size_t aId, const size_t bId){
        glm::ivec3 aPos = m_grid.getChunkWorldPosition(aId);
        int ax = gridCenter.x - aPos.x;
        int az = gridCenter.y - aPos.z;
        float aDist = std::sqrt(ax * ax + az * az);
        
        glm::ivec3 bPos = m_grid.getChunkWorldPosition(bId);
        int bx = gridCenter.x - bPos.x;
        int bz = gridCenter.y - bPos.z;
        float bDist = std::sqrt(bx * bx + bz * bz);
        
        return aDist > bDist;
    });
}

void engine::ChunkGridChanger::sortToGenerateQueue() {
    ChunkGridBounds& gridBounds = m_grid.getGridBounds();

    glm::ivec2 gridCenter = gridBounds.currentCenterChunk;
    sort(m_toGenerateQueue.begin(), m_toGenerateQueue.end(), [&](const glm::ivec2& aPos, const glm::ivec2& bPos){
        int ax = gridCenter.x - aPos.x;
        int az = gridCenter.y - aPos.y;
        float aDist = std::sqrt(ax * ax + az * az);
        
        int bx = gridCenter.x - bPos.x;
        int bz = gridCenter.y - bPos.y;
        float bDist = std::sqrt(bx * bx + bz * bz);
        
        return aDist > bDist;
    });
}
