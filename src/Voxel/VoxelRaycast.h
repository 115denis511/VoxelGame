#ifndef __VOXEL__VOXEL_RAYCAST_H__
#define __VOXEL__VOXEL_RAYCAST_H__

#include "../stdafx.h"
#include "ChunkGrid.h"
#include "ChunkGridBounds.h"
#include "VoxelPositionConverter.h"

namespace engine {
    class VoxelRaycast {
    public:
        VoxelRaycast() {}
        
        static bool raycastVoxel(
            ChunkGrid& grid, const ChunkGridBounds& bounds, 
            const glm::ivec3 &voxelPosition, const glm::vec3& direction, float maxDistance, glm::ivec3& hitPosition, glm::ivec3& hitFace
        );

    private:
        static int signum(float x);
        static float mod(int position, int modulus = 1);
        static float intbound(float s, float ds);
    };
}

#endif
