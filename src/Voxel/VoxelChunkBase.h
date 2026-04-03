#ifndef __VOXEL__VOXEL_CHUNK_BASE_H__
#define __VOXEL__VOXEL_CHUNK_BASE_H__

#include "../stdafx.h"
#include "../Utilites/MultidimensionalArrays.h"
#include "ChunkGridBounds.h"
#include "Voxel.h"

namespace engine {
    class VoxelChunkBase {
    public:
        VoxelChunkBase();
        virtual ~VoxelChunkBase();

        static constexpr int CHUNK_VOXELS_PER_AXIS = ChunkGridBounds::CHUNCK_DIMENSION_SIZE;
        static constexpr int MARCHING_CUBE_OVERLAPING_BORDER = 1;
        static constexpr int VOXELS_PER_AXIS = CHUNK_VOXELS_PER_AXIS + MARCHING_CUBE_OVERLAPING_BORDER;
        static constexpr int MARCHING_CUBES_PER_AXIS = VOXELS_PER_AXIS - 1;
        static constexpr int MARCHING_CUBES_COUNT = CHUNK_VOXELS_PER_AXIS * CHUNK_VOXELS_PER_AXIS * CHUNK_VOXELS_PER_AXIS;
        static constexpr int GRID_VOXEL_COUNT = VOXELS_PER_AXIS * VOXELS_PER_AXIS * VOXELS_PER_AXIS;
        static constexpr GLsizeiptr MARCHING_CUBES_BYTE_SIZE = MARCHING_CUBES_COUNT * sizeof(GLuint);
        static constexpr GLsizeiptr GRID_BYTE_SIZE = GRID_VOXEL_COUNT * sizeof(Voxel);

        void setVoxel(short x, short y, short z, const Voxel& voxel);
        void setSolidVoxel(short x, short y, short z, uint8_t id, uint8_t size = 3);
        void setLiquidVoxel(short x, short y, short z, uint8_t id, uint8_t size = 3);
        void deleteSolidVoxel(short x, short y, short z);
        void deleteLiquidVoxel(short x, short y, short z);
        Voxel getVoxel(short x, short y, short z);
        bool isVoxelSolid(short x, short y, short z);
        void clear();
        utilites::Array3D<Voxel, VOXELS_PER_AXIS, VOXELS_PER_AXIS, VOXELS_PER_AXIS>& getVoxelArray() { return m_voxels; }


    protected:
        utilites::Array3D<Voxel, VOXELS_PER_AXIS, VOXELS_PER_AXIS, VOXELS_PER_AXIS> m_voxels;
    };
}

#endif