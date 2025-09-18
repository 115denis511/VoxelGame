#include "VoxelChunk.h"

engine::VoxelChunk::VoxelChunk() {}

engine::VoxelChunk::~VoxelChunk() {}

engine::MarchingCubesVoxel engine::VoxelChunk::getVoxel(short x, short y, short z) {
    return m_voxels(x, y, z);
}

bool engine::VoxelChunk::isVoxelSolid(short x, short y, short z) {
    uint8_t id = m_voxels(x, y, z).id;
    return id < 128;
}

void engine::VoxelChunk::setVoxel(short x, short y, short z, uint8_t id, uint8_t size) {
    m_voxels(x, y, z).id = id;
    m_voxels(x, y, z).size = size < 8 ? size : 7;
}

void engine::VoxelChunk::clear() {
    for (size_t x = 0; x < VOXEL_CHUNK_SIZE; x++) {
        for (size_t y = 0; y < VOXEL_CHUNK_SIZE; y++) {
            for (size_t z = 0; z < VOXEL_CHUNK_SIZE; z++) {
                m_voxels(x, y, z) = MarchingCubesVoxel();
            }
        }
    }
}

void engine::VoxelChunk::addDrawCommand(const engine::DrawArraysIndirectCommand &command) {
    assert(m_drawCount < static_cast<int>(m_drawCommands.size()) && "ASSERT ERROR! - Adding more commands in chunk buffer than it can store!");
    m_drawCommands[m_drawCount] = command;
    m_drawCount++;
}

void engine::VoxelChunk::saveVisibilityStates(ChunkVisibilityState& visibilityStates) {
    constexpr uint32_t sidesCount = ChunkVisibilityState::getSidesCount();
    for (uint32_t i = 0; i < sidesCount; i++) {
        if (visibilityStates.isVisible(i)) {
            for (uint32_t j = 0; j < sidesCount; j++) {
                if (visibilityStates.isVisible(j)) {
                    m_visibilityStates[i].set(j);
                    m_visibilityStates[j].set(i);
                }
            }
        }
    }
    m_visibilityStates[(int)ChunkVisibilityState::Side::NONE].merge(visibilityStates);
}

void engine::VoxelChunk::clearVisibilityStates() {
    constexpr uint32_t SIDES_AND_NONE = ChunkVisibilityState::getSidesCount() + 1;
    for (size_t i = 0; i < SIDES_AND_NONE; i++) { 
        m_visibilityStates[i].clear();  
    }
}

void engine::VoxelChunk::clearVisibilityStatesForEmptyChunk() {
    constexpr uint32_t SIDES_AND_NONE = ChunkVisibilityState::getSidesCount() + 1;
    for (size_t i = 0; i < SIDES_AND_NONE; i++) { 
        m_visibilityStates[i].setAllVisible(); 
    }
}

/*
*   0 - 1   4 - 5
*   |   |   |   |
*   3 - 2   7 - 6
*    Top    Bottom
*/
/*uint8_t engine::MarchingCube::getCombinationId() {
    uint8_t result = static_cast<uint8_t>(voxelTopFrontLeft->isSolid);
    result = result | (voxelTopFrontRight->isSolid << 1);
    result = result | (voxelTopBackRight->isSolid << 2);
    result = result | (voxelTopBackLeft->isSolid << 3);
    result = result | (voxelBottomFrontLeft->isSolid << 4);
    result = result | (voxelBottomFrontRight->isSolid << 5);
    result = result | (voxelBottomBackRight->isSolid << 6);
    result = result | (voxelBottomBackLeft->isSolid << 7);
    return result;
}*/
