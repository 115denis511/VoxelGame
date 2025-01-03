#ifndef __VOXEL__VOXEL_CHUNK_H__
#define __VOXEL__VOXEL_CHUNK_H__

#include "../stdafx.h"
#include "../Render/DrawIndirectCommand.h"
#include "../Utilites/MultidimensionalArrays.h"
#include "../engine_properties.h"
#include "VoxelTriangleData.h"
#include "ChunkVisibilityState.h"

namespace engine {
    struct MarchingCubesVoxel {
        uint8_t id = 255;
        uint8_t size = 3;
    };

    class VoxelChunk { 
    public:
        VoxelChunk();
        ~VoxelChunk();
        
        enum class VisibilityPlane : uint32_t { 
            LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5
        };

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
        void updateVisibilityStates();
        void updateVisibilityStatesForEmptyChunk() { for (size_t i = 0; i < 27; i++) { m_visibilityStates[i].setAllVisible(); } };
        bool isVisibleThrough(ChunkVisibilityState::Side from, ChunkVisibilityState::Side to) { return m_visibilityStates[static_cast<int>(from)].isVisible(to); };

    private:
        enum class VisibilityCheckState : uint8_t { NOT_CHECKED = 0, CHECKED };

        static constexpr size_t VOXEL_CHUNK_SIZE = 32;
        static constexpr float VOXEL_SIZE = 1.f;
        utilites::Array3D<MarchingCubesVoxel, VOXEL_CHUNK_SIZE, VOXEL_CHUNK_SIZE, VOXEL_CHUNK_SIZE> m_voxels;
        GLuint m_commandBuffer, m_ssbo;
        int m_drawCount{ 0 };
        bool m_isInUpdateQueue{ false };
        bool m_isInUse{ false };
        ChunkVisibilityState m_visibilityStates[27];

        bool isPositionInsideChunk(short x, short y, short z) { 
            return x >= 0 && x < (short)VOXEL_CHUNK_SIZE && 
                   y >= 0 && y < (short)VOXEL_CHUNK_SIZE && 
                   z >= 0 && z < (short)VOXEL_CHUNK_SIZE; 
        };
        bool isVoxelEmptyAndNotChecked(short x, short y, short z, VisibilityCheckState (&state)[VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE]) {
            return !isVoxelSolid(x, y, z) && state[x][y][z] == VisibilityCheckState::NOT_CHECKED;
        };
        int floodFill(short x, short y, short z, VisibilityCheckState (&state)[VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE], ChunkVisibilityState& visabilityState);
        void floodFillScanNext(
            int lx, int rx, int y, int z, std::stack<glm::ivec3> &stack, VisibilityCheckState (&state)[VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE]
        );
        void saveVisibilityStates(ChunkVisibilityState& visibilityStates);
        void clearVisibilityStates();
    };
}

#endif