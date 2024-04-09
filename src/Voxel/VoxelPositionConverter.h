#ifndef __VOXEL__VOXEL_POSITION_CONVERTER_H__
#define __VOXEL__VOXEL_POSITION_CONVERTER_H__

#include "../stdafx.h"

namespace engine {
    class VoxelPositionConverter {
    public:
        VoxelPositionConverter() {}

        glm::ivec2 localChunkToWorldChunkPosition(int x, int z, int originX, int originZ);
        glm::ivec2 worldChunkToLocalChunkPosition(int x, int z, int originX, int originZ);
        glm::ivec3 worldPositionToChunkPosition(const glm::vec3& position, int chunkDimensionSize);
        glm::ivec3 worldPositionToVoxelPosition(const glm::vec3& position, int voxelSize);
        glm::ivec3 worldPositionToLocalVoxelPosition(const glm::ivec3& position, int chunkDimensionSize);

    private:

    };
}

#endif