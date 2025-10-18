#ifndef __VOXEL__VOXEL_CHUNK_BASE_H__
#define __VOXEL__VOXEL_CHUNK_BASE_H__

#include "../stdafx.h"
#include "../Utilites/MultidimensionalArrays.h"

namespace engine {
    struct MarchingCubesVoxel {
        uint8_t id = 255;
        uint8_t size = 3;
    };

    class VoxelChunkBase {
    public:
        VoxelChunkBase();
        virtual ~VoxelChunkBase();

        static constexpr int VOXEL_CHUNK_SIZE = 32;
        static constexpr int MARCHING_CUBE_OVERLAPING_BORDER = 1;
        static constexpr int GRID_SIZE = VOXEL_CHUNK_SIZE + MARCHING_CUBE_OVERLAPING_BORDER;
        static constexpr int VOXEL_COUNT = VOXEL_CHUNK_SIZE * VOXEL_CHUNK_SIZE * VOXEL_CHUNK_SIZE;
        static constexpr GLsizeiptr VOXEL_CHUNK_BYTE_SIZE = VOXEL_COUNT * sizeof(glm::ivec2);

        void setVoxel(short x, short y, short z, uint8_t id, uint8_t size = 3);
        void setVoxel(short x, short y, short z, const MarchingCubesVoxel& voxel);
        MarchingCubesVoxel getVoxel(short x, short y, short z);
        bool isVoxelSolid(short x, short y, short z);
        void clear();

    protected:
        utilites::Array3D<MarchingCubesVoxel, GRID_SIZE, GRID_SIZE, GRID_SIZE> m_voxels;
    };
}

#endif