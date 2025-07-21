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

        MarchingCubesVoxel getVoxel(short x, short y, short z);
        bool isVoxelSolid(short x, short y, short z);
        void setVoxel(short x, short y, short z, uint8_t id, uint8_t size = 3);
        void setInUpdateQueueFlag(bool flag) { m_isInUpdateQueue = flag; };
        void setInUseFlag(bool flag) { m_isInUse = flag; };
        void setMustClearOnUpdateFlag(bool flag) { m_mustClearOnUpdate = flag; };
        void clear();
        void bindSSBO(GLuint index);
        const GLuint getSSBO() { return m_ssbo; };
        void addDrawCommand(const DrawArraysIndirectCommand& command);
        const std::array<DrawArraysIndirectCommand, 254>& getDrawCommands() { return m_drawCommands; }
        const int getDrawCommandsCount() { return m_drawCount; };
        void clearDrawCommands() { m_drawCount = 0; };
        bool isInUpdateQueue() { return m_isInUpdateQueue; };
        bool isInUse() { return m_isInUse; };
        bool isMustClearOnUpdate() { return m_mustClearOnUpdate; };
        void updateVisibilityStates();
        void updateVisibilityStatesForEmptyChunk();
        bool isVisibleThrough(ChunkVisibilityState::Side from, ChunkVisibilityState::Side to) { return m_visibilityStates[static_cast<int>(from)].isVisible(to); };

    private:
        enum class VisibilityCheckState : uint8_t { NOT_CHECKED = 0, CHECKED };

        static constexpr size_t VOXEL_CHUNK_SIZE = 32;
        static constexpr float VOXEL_SIZE = 1.f;
        utilites::Array3D<MarchingCubesVoxel, VOXEL_CHUNK_SIZE, VOXEL_CHUNK_SIZE, VOXEL_CHUNK_SIZE> m_voxels;
        GLuint m_ssbo;
        std::array<DrawArraysIndirectCommand, 254> m_drawCommands;
        int m_drawCount{ 0 };
        bool m_isInUpdateQueue{ false };
        bool m_isInUse{ false };
        bool m_mustClearOnUpdate{ false };
        ChunkVisibilityState m_visibilityStates[ChunkVisibilityState::getSidesCount() + 1];

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