#ifndef __VOXEL__VOXEL_CHUNK_H__
#define __VOXEL__VOXEL_CHUNK_H__

#include "../stdafx.h"
#include "../Render/DrawIndirectCommand.h"
#include "../Render/ShaderStorageBuffer.h"
#include "../Utilites/MultidimensionalArrays.h"
#include "VoxelChunkBase.h"
#include "VoxelTriangleData.h"
#include "ChunkVisibilityState.h"

namespace engine {
    class ChunkGrid;

    class VoxelChunk : public VoxelChunkBase { 
        friend ChunkGrid;
    public:
        VoxelChunk();
        ~VoxelChunk();

        void setInUpdateQueueFlag(bool flag) { m_isInUpdateQueue = flag; };
        void setInUseFlag(bool flag) { m_isInUse = flag; };
        void bindSSBO(GLuint blockId) { m_ssbo.bind(blockId); }
        ShaderStorageBuffer<glm::ivec2>& getSSBO() { return m_ssbo; };
        const GLuint getIdInSSBO() { return m_idInSSBO; };
        void addDrawCommand(const DrawArraysIndirectCommand& command);
        const std::array<DrawArraysIndirectCommand, 254>& getDrawCommands() { return m_drawCommands; }
        const int getDrawCommandsCount() { return m_drawCount; };
        void clearDrawCommands() { m_drawCount = 0; };
        bool isInUpdateQueue() { return m_isInUpdateQueue; };
        bool isInUse() { return m_isInUse; };
        bool isVisibleThrough(ChunkVisibilityState::Side from, ChunkVisibilityState::Side to) { return m_visibilityStates[static_cast<int>(from)].isVisible(to); };
        void saveVisibilityStates(ChunkVisibilityState& visibilityStates);
        void clearVisibilityStates();
        void clearVisibilityStatesForEmptyChunk();

    private:
        std::array<DrawArraysIndirectCommand, 254> m_drawCommands;
        int m_drawCount{ 0 };
        bool m_isInUpdateQueue{ false };
        bool m_isInUse{ false };
        ChunkVisibilityState m_visibilityStates[ChunkVisibilityState::getSidesCount() + 1];
        union {
            ShaderStorageBuffer<glm::ivec2> m_ssbo;
            GLuint m_idInSSBO;
        };

        void initSSBO() { m_ssbo.init(VOXEL_COUNT, BufferUsage::DYNAMIC_DRAW); }
        void freeSSBO() { m_ssbo.~ShaderStorageBuffer<glm::ivec2>(); }
        void setIdInSSBO(GLuint start) { m_idInSSBO = start; }
        bool isPositionInsideChunk(short x, short y, short z) { 
            return x >= 0 && x < (short)VOXEL_CHUNK_SIZE && 
                   y >= 0 && y < (short)VOXEL_CHUNK_SIZE && 
                   z >= 0 && z < (short)VOXEL_CHUNK_SIZE; 
        };
    };
}

#endif