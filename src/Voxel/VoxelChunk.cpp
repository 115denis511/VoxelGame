#include "VoxelChunk.h"

engine::VoxelChunk::VoxelChunk() {
    glGenBuffers(1, &m_commandBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, (254 * sizeof(DrawArraysIndirectCommand)), NULL, GL_DYNAMIC_DRAW);  
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    glCreateBuffers(1, &m_ssbo);
    constexpr GLuint byteSize = 31 * 31 * 31 * sizeof(glm::ivec2);
    glNamedBufferData(m_ssbo, byteSize, NULL, GL_DYNAMIC_DRAW);
}

engine::VoxelChunk::~VoxelChunk() {
    glDeleteBuffers(1, &m_commandBuffer);
    glDeleteBuffers(1, &m_ssbo);
}

engine::MarchingCubesVoxel engine::VoxelChunk::getVoxel(short x, short y, short z) {
    return m_voxels(x, y, z);
}

bool engine::VoxelChunk::isVoxelSolid(short x, short y, short z) {
    return m_voxels(x, y, z).id < 128;
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

void engine::VoxelChunk::bindCommandBuffer() {
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);
}

void engine::VoxelChunk::bindSSBO() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, engine_properties::SSBO_MARCHING_CUBES_CHUNK_DATA_BLOCK_ID, m_ssbo);
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
