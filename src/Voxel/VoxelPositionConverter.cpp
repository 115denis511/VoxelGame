#include "VoxelPositionConverter.h"

glm::ivec2 engine::VoxelPositionConverter::localChunkToWorldChunkPosition(int x, int z, int originX, int originZ) {
    return glm::ivec2(x + originX, z + originZ);
}

glm::ivec2 engine::VoxelPositionConverter::worldChunkToLocalChunkPosition(int x, int z, int originX, int originZ) {
    return glm::ivec2(x - originX, z - originZ);
}

glm::ivec3 engine::VoxelPositionConverter::worldPositionToChunkPosition(const glm::vec3 &position, int chunkDimensionSize) {
    return glm::ivec3(std::floor(position.x / chunkDimensionSize), std::floor(position.y / chunkDimensionSize), std::floor(position.z / chunkDimensionSize));
}

glm::ivec3 engine::VoxelPositionConverter::worldPositionToVoxelPosition(const glm::vec3 &position, int voxelSize) {
    return glm::ivec3(std::floor(position.x / voxelSize), std::floor(position.y / voxelSize), std::floor(position.z / voxelSize)); 
}

glm::ivec3 engine::VoxelPositionConverter::worldPositionToLocalVoxelPosition(const glm::ivec3 &position, int chunkDimensionSize) {
    glm::ivec3 local;
    local.x = position.x % chunkDimensionSize;
    if (local.x < 0) {
        local.x += chunkDimensionSize;
    }
    local.y = position.y % chunkDimensionSize;
    if (local.y < 0) {
        local.y += chunkDimensionSize;
    }
    local.z = position.z % chunkDimensionSize;
    if (local.z < 0) {
        local.z += chunkDimensionSize;
    }
    return local;
}
