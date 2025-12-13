#include "VoxelChunkBase.h"

engine::VoxelChunkBase::VoxelChunkBase() {}

engine::VoxelChunkBase::~VoxelChunkBase() {}

void engine::VoxelChunkBase::setVoxel(short x, short y, short z, const Voxel& voxel) {
    m_voxels(x, y, z) = voxel;
}

void engine::VoxelChunkBase::setSolidVoxel(short x, short y, short z, uint8_t id, uint8_t size) {
    m_voxels(x, y, z).setSolidVoxel(id, size);
}

void engine::VoxelChunkBase::setLiquidVoxel(short x, short y, short z, uint8_t id, uint8_t size) {
    m_voxels(x, y, z).setLiquidVoxel(id, size);
}

engine::Voxel engine::VoxelChunkBase::getVoxel(short x, short y, short z) {
    return m_voxels(x, y, z);
}

bool engine::VoxelChunkBase::isVoxelSolid(short x, short y, short z) {
    return m_voxels(x, y, z).isHaveSolid();
}

void engine::VoxelChunkBase::clear() {
    for (size_t x = 0; x < GRID_SIZE; x++) {
        for (size_t y = 0; y < GRID_SIZE; y++) {
            for (size_t z = 0; z < GRID_SIZE; z++) {
                m_voxels(x, y, z) = Voxel();
            }
        }
    }
}