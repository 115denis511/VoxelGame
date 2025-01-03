#include "ChunkGridBounds.h"

bool engine::ChunkGridBounds::isChunkInbounds(int x, int y, int z) const {
    int maxX = currentOriginChunk.x + usedChunkGridWidth;
    int maxZ = currentOriginChunk.y + usedChunkGridWidth;
    int maxY = ChunkGridBounds::CHUNK_MAX_Y_SIZE;
    bool result = x >= currentOriginChunk.x && x < maxX && z >= currentOriginChunk.y && z < maxZ && y >= 0 && y < maxY;
    return result;
}

bool engine::ChunkGridBounds::isPositionInbounds(const glm::vec3 &position) const {
    int minX = currentOriginChunk.x * CHUNCK_DIMENSION_SIZE;
    int minZ = currentOriginChunk.y * CHUNCK_DIMENSION_SIZE;
    int minY = 0;
    int maxX = minX + (usedChunkGridWidth * CHUNCK_DIMENSION_SIZE);
    int maxZ = minZ + (usedChunkGridWidth * CHUNCK_DIMENSION_SIZE);
    int maxY = minY + (CHUNK_MAX_Y_SIZE * CHUNCK_DIMENSION_SIZE);
    return position.x >= minX && position.x < maxX && position.y >= minY && position.y < maxY && position.z >= minZ && position.z < maxZ;
}