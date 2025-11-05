#ifndef __VOXEL__VOXEL_CHUNK_BASE_H__
#define __VOXEL__VOXEL_CHUNK_BASE_H__

#include "../stdafx.h"
#include "../Utilites/MultidimensionalArrays.h"
#include "Voxel.h"

namespace engine {
    class VoxelChunkBase {
    public:
        VoxelChunkBase();
        virtual ~VoxelChunkBase();

        static constexpr int VOXEL_CHUNK_SIZE = 32;
        static constexpr int MARCHING_CUBE_OVERLAPING_BORDER = 1;
        static constexpr int GRID_SIZE = VOXEL_CHUNK_SIZE + MARCHING_CUBE_OVERLAPING_BORDER;
        static constexpr int MARCHING_CUBES_COUNT = VOXEL_CHUNK_SIZE * VOXEL_CHUNK_SIZE * VOXEL_CHUNK_SIZE;
        static constexpr int GRID_VOXEL_COUNT = GRID_SIZE * GRID_SIZE * GRID_SIZE;
        static constexpr GLsizeiptr MARCHING_CUBES_BYTE_SIZE = MARCHING_CUBES_COUNT * sizeof(glm::ivec2);
        static constexpr GLsizeiptr GRID_BYTE_SIZE = GRID_VOXEL_COUNT * sizeof(Voxel);

        void setVoxel(short x, short y, short z, uint8_t id, uint8_t size = 3);
        void setVoxel(short x, short y, short z, const Voxel& voxel);
        Voxel getVoxel(short x, short y, short z);
        bool isVoxelSolid(short x, short y, short z);
        void clear();
        utilites::Array3D<Voxel, GRID_SIZE, GRID_SIZE, GRID_SIZE>& getVoxelArray() { return m_voxels; }


    protected:
        utilites::Array3D<Voxel, GRID_SIZE, GRID_SIZE, GRID_SIZE> m_voxels;
    };
}

#endif