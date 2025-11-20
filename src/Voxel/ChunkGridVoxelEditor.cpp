#include "ChunkGridVoxelEditor.h"

void engine::ChunkGridVoxelEditor::setVoxel(const glm::vec3 &position, uint8_t id, uint8_t size, Voxel::Type type) {
    glm::ivec3 chunkPos = m_converter.worldPositionToChunkPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);
    glm::ivec2 localXZ = m_converter.worldChunkToLocalChunkPosition(chunkPos.x, chunkPos.z, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
    if (
        !m_gridBounds.isWorldChunkInbounds(chunkPos.x, chunkPos.y, chunkPos.z) ||
        !m_grid.isHaveChunk(localXZ.x, chunkPos.y, localXZ.y)
    ) return;
    glm::ivec3 localVoxel = m_converter.worldPositionToLocalVoxelPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);

    size_t chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y, localXZ.y);
    VoxelChunk& chunk = m_grid.getChunk(chunkId);
    m_gridChanger.pushToUpdateQueue(chunkId);
    chunk.setVoxel(localVoxel.x, localVoxel.y, localVoxel.z, id, size, type);

    if (localVoxel.x == 0) {
        glm::ivec3 chunkX = glm::ivec3(chunkPos.x - 1, chunkPos.y, chunkPos.z);
        if (m_gridBounds.isWorldChunkInbounds(chunkX.x, chunkX.y, chunkX.z) && m_grid.isHaveChunk(localXZ.x - 1, chunkPos.y, localXZ.y)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y, localXZ.y);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            heighbour.setVoxel(32, localVoxel.y, localVoxel.z, id, size, type);
        }
    }

    if (localVoxel.z == 0) {
        glm::ivec3 chunkZ = glm::ivec3(chunkPos.x, chunkPos.y, chunkPos.z - 1);
        if (m_gridBounds.isWorldChunkInbounds(chunkZ.x, chunkZ.y, chunkZ.z) && m_grid.isHaveChunk(localXZ.x, chunkPos.y, localXZ.y - 1)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y, localXZ.y - 1);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            heighbour.setVoxel(localVoxel.x, localVoxel.y, 32, id, size, type);
        }
    }

    if (localVoxel.y == 0) {
        glm::ivec3 chunkY = glm::ivec3(chunkPos.x, chunkPos.y - 1, chunkPos.z);
        if (m_gridBounds.isWorldChunkInbounds(chunkY.x, chunkY.y, chunkY.z) && m_grid.isHaveChunk(localXZ.x, chunkPos.y - 1, localXZ.y)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y - 1, localXZ.y);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            heighbour.setVoxel(localVoxel.x, 32, localVoxel.z, id, size, type);
        }
    }

    if (localVoxel.x == 0 && localVoxel.z == 0) {
        glm::ivec3 chunkXZ = glm::ivec3(chunkPos.x - 1, chunkPos.y, chunkPos.z - 1);
        if (m_gridBounds.isWorldChunkInbounds(chunkXZ.x, chunkXZ.y, chunkXZ.z) && m_grid.isHaveChunk(localXZ.x - 1, chunkPos.y, localXZ.y - 1)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y, localXZ.y - 1);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            heighbour.setVoxel(32, localVoxel.y, 32, id, size, type);
        }
    }

    if (localVoxel.x == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkXY = glm::ivec3(chunkPos.x - 1, chunkPos.y - 1, chunkPos.z);
        if (m_gridBounds.isWorldChunkInbounds(chunkXY.x, chunkXY.y, chunkXY.z) && m_grid.isHaveChunk(localXZ.x - 1, chunkPos.y - 1, localXZ.y)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y - 1, localXZ.y);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            heighbour.setVoxel(32, 32, localVoxel.z, id, size, type);
        }
    }

    if (localVoxel.z == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkYZ = glm::ivec3(chunkPos.x, chunkPos.y - 1, chunkPos.z - 1);
        if (m_gridBounds.isWorldChunkInbounds(chunkYZ.x, chunkYZ.y, chunkYZ.z) && m_grid.isHaveChunk(localXZ.x, chunkPos.y - 1, localXZ.y - 1)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y - 1, localXZ.y - 1);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            heighbour.setVoxel(localVoxel.x, 32, 32, id, size, type);
        }
    }

    if (localVoxel.x == 0 && localVoxel.z == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkXYZ = glm::ivec3(chunkPos.x - 1, chunkPos.y - 1, chunkPos.z - 1);
        if (m_gridBounds.isWorldChunkInbounds(chunkXYZ.x, chunkXYZ.y, chunkXYZ.z) && m_grid.isHaveChunk(localXZ.x - 1, chunkPos.y - 1, localXZ.y - 1)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y - 1, localXZ.y - 1);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& heighbour = m_grid.getChunk(chunkId);
            heighbour.setVoxel(32, 32, 32, id, size, type);
        }
    }
}