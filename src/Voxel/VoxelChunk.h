#ifndef __VOXEL__VOXEL_CHUNK_H__
#define __VOXEL__VOXEL_CHUNK_H__

#include "../stdafx.h"
#include "../Render/DrawIndirectCommand.h"
#include "../Utilites/MultidimensionalArrays.h"
#include "../engine_properties.h"
#include "VoxelVertexData.h"

namespace engine {
    struct MarchingCubesVoxel {
        uint8_t id = 255;
        uint8_t size = 3;
    };

    class VoxelChunk { 
    public:
        VoxelChunk();
        ~VoxelChunk();

        MarchingCubesVoxel getVoxel(short x, short y, short z);
        bool isVoxelSolid(short x, short y, short z);
        void setVoxel(short x, short y, short z, uint8_t id, uint8_t size = 3);
        void setDrawCount(int drawCount) { m_drawCount = drawCount; };
        void setInUpdateQueueFlag(bool flag) { m_isInUpdateQueue = flag; };
        void setInUseFlag(bool flag) { m_isInUse = flag; };
        void clear();
        void bindCommandBuffer();
        void bindSSBO();
        const GLuint getCommandBuffer() { return m_commandBuffer; };
        const GLuint getSSBO() { return m_ssbo; };
        const int getDrawCount() { return m_drawCount; };
        void clearDrawCount() { m_drawCount = 0; };
        bool isInUpdateQueue() { return m_isInUpdateQueue; };
        bool isInUse() { return m_isInUse; };

    private:
        static constexpr size_t VOXEL_CHUNK_SIZE = 32;
        static constexpr float VOXEL_SIZE = 1.f;
        utilites::Array3D<MarchingCubesVoxel, VOXEL_CHUNK_SIZE, VOXEL_CHUNK_SIZE, VOXEL_CHUNK_SIZE> m_voxels;
        GLuint m_commandBuffer, m_ssbo;
        int m_drawCount{ 0 };
        bool m_isInUpdateQueue{ false };
        bool m_isInUse{ false };
    };
}

#endif