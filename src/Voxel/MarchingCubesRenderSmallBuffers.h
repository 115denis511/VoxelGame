#ifndef __VOXEL__MARCHING_CUBES_RENDER_SMALL_BUFFERS_H__
#define __VOXEL__MARCHING_CUBES_RENDER_SMALL_BUFFERS_H__

#include "../stdafx.h"
#include "ChunkGrid.h"
#include "ChunkGridBounds.h"
#include "ChunkGridVisibility.h"
#include "MarchingCubes.h"
#include "MarchingCubesRenderBase.h"
#include "MarchingCubesSSBOs.h"
#include "VoxelChunk.h"

namespace engine {
    class MarchingCubesRenderSmallBuffers : public MarchingCubesRenderBase {
    public:
        MarchingCubesRenderSmallBuffers(
            ChunkGrid& grid,
            ChunkGridBounds& gridBounds,
            ChunkGridVisibility& gridVisibility,
            MarchingCubesSSBOs& ssbos
        );

        virtual void initSSBOs(MarchingCubesSSBOs& ssbos) override;
        virtual void prepareToDraw(const CameraVars &cameraVars, Frustum frustum) override;
        virtual void drawSolid(MarchingCubes& marchingCubes) override;
        virtual void drawLiquid(MarchingCubes& marchingCubes) override;

    private:
        static constexpr GLuint SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS = 14;
        static constexpr GLuint SSBO_BLOCK__DRAW_ID_TO_CHUNK = 15;
        static constexpr GLuint CHUNK_BATCH_MAX_SIZE = 7;
        static constexpr GLuint BATCHES_COUNT = ChunkGridBounds::CHUNK_COUNT / CHUNK_BATCH_MAX_SIZE + 1;

        struct DrawBatch {
            VoxelChunk* chunksToDraw[CHUNK_BATCH_MAX_SIZE] = { nullptr };
            std::vector<GLuint> bufferRefs{ std::vector<GLuint>(CHUNK_BATCH_MAX_SIZE * 254) };
            std::vector<glm::vec4> chunkPositions{ std::vector<glm::vec4>(CHUNK_BATCH_MAX_SIZE) };
            std::vector<DrawArraysIndirectCommand> drawCommands{ std::vector<DrawArraysIndirectCommand>(CHUNK_BATCH_MAX_SIZE * 254) };
            GLuint drawCommandsCount{ 0 };
            GLuint chunkCount{ 0 };

            void clear() {
                drawCommandsCount = 0;
                chunkCount = 0;
            }
        };

        ChunkGrid& m_grid;
        ChunkGridBounds& m_gridBounds;
        ChunkGridVisibility& m_gridVisibility;
        MarchingCubesSSBOs& m_ssbos;
        
        GLuint m_solidBatchesCount{ 0 };
        GLuint m_liquidBatchesCount{ 0 };
        std::vector<DrawBatch> m_solidBatches;
        std::vector<DrawBatch> m_liquidBatches;
    };
}

#endif