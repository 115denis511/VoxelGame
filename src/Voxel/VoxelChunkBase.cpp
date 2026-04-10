#include "VoxelChunkBase.h"

engine::VoxelChunkBase::VoxelChunkBase() {}

engine::VoxelChunkBase::~VoxelChunkBase() {}

void engine::VoxelChunkBase::setVoxel(short x, short y, short z, const Voxel& voxel) {
    m_voxels(x, y, z) = voxel;
    m_emptyVoxelsGrid.set(x, y, z, !voxel.isHaveSolid());
}

void engine::VoxelChunkBase::setSolidVoxel(short x, short y, short z, uint8_t id, uint8_t size) {
    assert(id < Voxel::NO_VOXEL);
    m_voxels(x, y, z).setSolidVoxel(id, size);
    m_emptyVoxelsGrid.unset(x, y, z);
}

void engine::VoxelChunkBase::setLiquidVoxel(short x, short y, short z, uint8_t id, uint8_t size) {
    assert(id < Voxel::NO_VOXEL);
    m_voxels(x, y, z).setLiquidVoxel(id, size);
}

void engine::VoxelChunkBase::deleteSolidVoxel(short x, short y, short z) {
    m_voxels(x, y, z).deleteSolid();
    m_emptyVoxelsGrid.set(x, y, z);
}

void engine::VoxelChunkBase::deleteLiquidVoxel(short x, short y, short z) {
    m_voxels(x, y, z).deleteLiquid();
}

engine::Voxel engine::VoxelChunkBase::getVoxel(short x, short y, short z) {
    return m_voxels(x, y, z);
}

bool engine::VoxelChunkBase::isVoxelSolid(short x, short y, short z) {
    return m_voxels(x, y, z).isHaveSolid();
}

void engine::VoxelChunkBase::clear() {
    for (size_t x = 0; x < VOXELS_PER_AXIS; x++) {
        for (size_t y = 0; y < VOXELS_PER_AXIS; y++) {
            for (size_t z = 0; z < VOXELS_PER_AXIS; z++) {
                m_voxels(x, y, z) = Voxel();
            }
        }
    }

    m_emptyVoxelsGrid.fillAllRows(~0);
}