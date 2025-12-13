#include "ChunkGridVoxelEditor.h"

void engine::ChunkGridVoxelEditor::setSolidVoxel(const glm::vec3 &position, uint8_t id, uint8_t size) {
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
    chunk.setSolidVoxel(localVoxel.x, localVoxel.y, localVoxel.z, id, size);
    
    Voxel voxel = chunk.getVoxel(localVoxel.x, localVoxel.y, localVoxel.z);
    syncChunkBorders(chunkPos, localXZ, localVoxel, voxel);
}

void engine::ChunkGridVoxelEditor::setLiquidVoxel(const glm::vec3 &position, uint8_t id, uint8_t size) {
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
    chunk.setLiquidVoxel(localVoxel.x, localVoxel.y, localVoxel.z, id, size);
    
    Voxel voxel = chunk.getVoxel(localVoxel.x, localVoxel.y, localVoxel.z);
    syncChunkBorders(chunkPos, localXZ, localVoxel, voxel);
}

void engine::ChunkGridVoxelEditor::deleteSolidVoxel(const glm::vec3 &position) {
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
    chunk.deleteSolidVoxel(localVoxel.x, localVoxel.y, localVoxel.z);
    
    Voxel voxel = chunk.getVoxel(localVoxel.x, localVoxel.y, localVoxel.z);
    syncChunkBorders(chunkPos, localXZ, localVoxel, voxel);
}

void engine::ChunkGridVoxelEditor::deleteLiquidVoxel(const glm::vec3 &position) {
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
    chunk.deleteLiquidVoxel(localVoxel.x, localVoxel.y, localVoxel.z);
    
    Voxel voxel = chunk.getVoxel(localVoxel.x, localVoxel.y, localVoxel.z);
    syncChunkBorders(chunkPos, localXZ, localVoxel, voxel);
}

void engine::ChunkGridVoxelEditor::syncChunkBorders(const glm::ivec3 &chunkWorldPos, const glm::ivec2 &chunkLocalPos, const glm::ivec3& localVoxelPos, const Voxel &voxel) {
    constexpr unsigned int X_BIT = 0b001;
    constexpr unsigned int Y_BIT = 0b010;
    constexpr unsigned int Z_BIT = 0b100;

    bool onXBorder = localVoxelPos.x == 0;
    bool onYBorder = localVoxelPos.y == 0;
    bool onZBorder = localVoxelPos.z == 0;
    unsigned int bitmask = onXBorder | (onYBorder << 1) | (onZBorder << 2);

    switch (bitmask) {
    case X_BIT:
        syncLeftChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.y, localVoxelPos.z);
        break;

    case Y_BIT:
        syncBottomChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x, localVoxelPos.z);
        break;

    case Z_BIT:
        syncBackChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x, localVoxelPos.y);
        break;

    case X_BIT | Y_BIT:
        syncLeftChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.y, localVoxelPos.z);
        syncBottomChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x, localVoxelPos.z);
        syncLeftBottomChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.z);
        break;

    case X_BIT | Z_BIT:
        syncLeftChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.y, localVoxelPos.z);
        syncBackChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x, localVoxelPos.y);
        syncLeftBackChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.y);
        break;

    case Y_BIT | Z_BIT:
        syncBottomChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x, localVoxelPos.z);
        syncBackChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x, localVoxelPos.y);
        syncBottomBackChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x);
        break;

    case X_BIT | Y_BIT | Z_BIT:
        syncLeftChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.y, localVoxelPos.z);
        syncBottomChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x, localVoxelPos.z);
        syncBackChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x, localVoxelPos.y);
        syncLeftBottomChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.z);
        syncLeftBackChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.y);
        syncBottomBackChunk(chunkWorldPos, chunkLocalPos, voxel, localVoxelPos.x);
        syncLeftBottomBackChunk(chunkWorldPos, chunkLocalPos, voxel);
        break;
    
    default:
        break;
    }
}

void engine::ChunkGridVoxelEditor::syncLeftChunk(const glm::ivec3 &chunkWorldPos, const glm::ivec2 &chunkLocalPos, const Voxel &voxel, int y, int z)
{
    if (
        m_gridBounds.isWorldChunkInbounds(chunkWorldPos.x - 1, chunkWorldPos.y, chunkWorldPos.z) && 
        m_grid.isHaveChunk(chunkLocalPos.x - 1, chunkWorldPos.y, chunkLocalPos.y)
    ) {
        int chunkId = m_grid.getChunkId(chunkLocalPos.x - 1, chunkWorldPos.y, chunkLocalPos.y);
        m_gridChanger.pushToUpdateQueue(chunkId);

        VoxelChunk& heighbour = m_grid.getChunk(chunkId);
        heighbour.setVoxel(32, y, z, voxel);
    }
}

void engine::ChunkGridVoxelEditor::syncBottomChunk(const glm::ivec3 &chunkWorldPos, const glm::ivec2 &chunkLocalPos, const Voxel &voxel, int x, int z) {
    if (
        m_gridBounds.isWorldChunkInbounds(chunkWorldPos.x, chunkWorldPos.y - 1, chunkWorldPos.z) && 
        m_grid.isHaveChunk(chunkLocalPos.x, chunkWorldPos.y - 1, chunkLocalPos.y)
    ) {
        int chunkId = m_grid.getChunkId(chunkLocalPos.x, chunkWorldPos.y - 1, chunkLocalPos.y);
        m_gridChanger.pushToUpdateQueue(chunkId);

        VoxelChunk& heighbour = m_grid.getChunk(chunkId);
        heighbour.setVoxel(x, 32, z, voxel);
    }
}

void engine::ChunkGridVoxelEditor::syncBackChunk(const glm::ivec3 &chunkWorldPos, const glm::ivec2 &chunkLocalPos, const Voxel &voxel, int x, int y) {
    if (
        m_gridBounds.isWorldChunkInbounds(chunkWorldPos.x, chunkWorldPos.y, chunkWorldPos.z - 1) && 
        m_grid.isHaveChunk(chunkLocalPos.x, chunkWorldPos.y, chunkLocalPos.y - 1)
    ) {
        int chunkId = m_grid.getChunkId(chunkLocalPos.x, chunkWorldPos.y, chunkLocalPos.y - 1);
        m_gridChanger.pushToUpdateQueue(chunkId);

        VoxelChunk& heighbour = m_grid.getChunk(chunkId);
        heighbour.setVoxel(x, y, 32, voxel);
    }
}

void engine::ChunkGridVoxelEditor::syncLeftBottomChunk(const glm::ivec3 &chunkWorldPos, const glm::ivec2 &chunkLocalPos, const Voxel &voxel, int z) {
    if (
        m_gridBounds.isWorldChunkInbounds(chunkWorldPos.x - 1, chunkWorldPos.y - 1, chunkWorldPos.z) && 
        m_grid.isHaveChunk(chunkLocalPos.x - 1, chunkWorldPos.y - 1, chunkLocalPos.y)
    ) {
        int chunkId = m_grid.getChunkId(chunkLocalPos.x - 1, chunkWorldPos.y - 1, chunkLocalPos.y);
        m_gridChanger.pushToUpdateQueue(chunkId);

        VoxelChunk& heighbour = m_grid.getChunk(chunkId);
        heighbour.setVoxel(32, 32, z, voxel);
    }
}

void engine::ChunkGridVoxelEditor::syncLeftBackChunk(const glm::ivec3 &chunkWorldPos, const glm::ivec2 &chunkLocalPos, const Voxel &voxel, int y) {
    if (
        m_gridBounds.isWorldChunkInbounds(chunkWorldPos.x - 1, chunkWorldPos.y, chunkWorldPos.z - 1) && 
        m_grid.isHaveChunk(chunkLocalPos.x - 1, chunkWorldPos.y, chunkLocalPos.y - 1)
    ) {
        int chunkId = m_grid.getChunkId(chunkLocalPos.x - 1, chunkWorldPos.y, chunkLocalPos.y - 1);
        m_gridChanger.pushToUpdateQueue(chunkId);

        VoxelChunk& heighbour = m_grid.getChunk(chunkId);
        heighbour.setVoxel(32, y, 32, voxel);
    }
}

void engine::ChunkGridVoxelEditor::syncBottomBackChunk(const glm::ivec3 &chunkWorldPos, const glm::ivec2 &chunkLocalPos, const Voxel &voxel, int x) {
    if (
        m_gridBounds.isWorldChunkInbounds(chunkWorldPos.x, chunkWorldPos.y - 1, chunkWorldPos.z - 1) && 
        m_grid.isHaveChunk(chunkLocalPos.x, chunkWorldPos.y - 1, chunkLocalPos.y - 1)
    ) {
        int chunkId = m_grid.getChunkId(chunkLocalPos.x, chunkWorldPos.y - 1, chunkLocalPos.y - 1);
        m_gridChanger.pushToUpdateQueue(chunkId);

        VoxelChunk& heighbour = m_grid.getChunk(chunkId);
        heighbour.setVoxel(x, 32, 32, voxel);
    }
}

void engine::ChunkGridVoxelEditor::syncLeftBottomBackChunk(const glm::ivec3 &chunkWorldPos, const glm::ivec2 &chunkLocalPos, const Voxel &voxel) {
    if (
        m_gridBounds.isWorldChunkInbounds(chunkWorldPos.x - 1, chunkWorldPos.y - 1, chunkWorldPos.z - 1) && 
        m_grid.isHaveChunk(chunkLocalPos.x - 1, chunkWorldPos.y - 1, chunkLocalPos.y - 1)
    ) {
        int chunkId = m_grid.getChunkId(chunkLocalPos.x - 1, chunkWorldPos.y - 1, chunkLocalPos.y - 1);
        m_gridChanger.pushToUpdateQueue(chunkId);

        VoxelChunk& heighbour = m_grid.getChunk(chunkId);
        heighbour.setVoxel(32, 32, 32, voxel);
    }
}
