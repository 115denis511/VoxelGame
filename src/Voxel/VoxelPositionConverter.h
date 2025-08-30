#ifndef __VOXEL__VOXEL_POSITION_CONVERTER_H__
#define __VOXEL__VOXEL_POSITION_CONVERTER_H__

#include "../stdafx.h"

namespace engine {
    class VoxelPositionConverter {
    public:
        VoxelPositionConverter() {}

        static glm::ivec2 localChunkToWorldChunkPosition(int x, int z, int originX, int originZ);
        static glm::ivec2 worldChunkToLocalChunkPosition(int x, int z, int originX, int originZ);
        static glm::ivec3 worldChunkToLocalChunkPosition(int x, int y, int z, int originX, int originZ);
        static glm::ivec3 worldChunkToLocalChunkPosition(const glm::ivec3 &chunkPosition, const glm::ivec2 &originPosition) { 
            return worldChunkToLocalChunkPosition(chunkPosition.x, chunkPosition.y, chunkPosition.z, originPosition.x, originPosition.y); 
        };
        static glm::ivec3 worldPositionToChunkPosition(const glm::vec3& position, int chunkDimensionSize);
        static glm::ivec3 worldPositionToVoxelPosition(const glm::vec3& position, int voxelSize);
        static glm::ivec3 worldPositionToLocalVoxelPosition(const glm::ivec3& worldVoxel, int chunkDimensionSize);

    private:

    };
}

#endif