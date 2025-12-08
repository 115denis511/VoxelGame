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
        virtual void drawSolid(const CameraVars &cameraVars, Frustum frustum, MarchingCubes& marchingCubes) override;

    private:
        static constexpr GLuint SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS = 14;
        static constexpr GLuint SSBO_BLOCK__DRAW_ID_TO_CHUNK = 15;
        static constexpr GLuint CHUNK_BATCH_MAX_SIZE = 7;

        ChunkGrid& m_grid;
        ChunkGridBounds& m_gridBounds;
        ChunkGridVisibility& m_gridVisibility;
        MarchingCubesSSBOs& m_ssbos;
        
        std::vector<glm::vec4> m_solidsDrawChunkPositions;
        std::vector<glm::vec4> m_liquidsDrawChunkPositions;
        std::vector<VoxelChunk*> m_solidsChunksToDraw;
        std::vector<VoxelChunk*> m_liquidsChunksToDraw;
        GLuint m_solidsBufferIndex{ 0 };
        GLuint m_liquidsBufferIndex{ 0 };

        void drawSolidsAccumulatedBatches(MarchingCubes &marchingCubes, GLsizei solidsDrawCount);
        void drawLiquidsAccumulatedBatches(MarchingCubes &marchingCubes, GLsizei liquidsDrawCount);
    };
}

#endif