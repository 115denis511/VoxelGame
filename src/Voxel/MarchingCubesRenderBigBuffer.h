#ifndef __VOXEL__MARCHING_CUBES_RENDER_BIG_BUFFER_H__
#define __VOXEL__MARCHING_CUBES_RENDER_BIG_BUFFER_H__

#include "../stdafx.h"
#include "ChunkGrid.h"
#include "ChunkGridBounds.h"
#include "ChunkGridVisibility.h"
#include "MarchingCubes.h"
#include "MarchingCubesRenderBase.h"
#include "MarchingCubesSSBOs.h"
#include "VoxelChunk.h"

namespace engine {
    class MarchingCubesRenderBigBuffer : public MarchingCubesRenderBase {
    public:
        MarchingCubesRenderBigBuffer(
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
        static constexpr GLuint SSBO_BLOCK__GLOBAL_CHUNK_GRIDS_STORAGE = 11;
        static constexpr GLuint SSBO_BLOCK__GLOBAL_CHUNK_STORAGE = 12;
        static constexpr GLuint SSBO_BLOCK__CHUNK_POSITIONS = 13;
        static constexpr GLuint SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS = 14;
        static constexpr GLuint SSBO_BLOCK__DRAW_ID_TO_CHUNK = 15;
        
        ChunkGrid& m_grid;
        ChunkGridBounds& m_gridBounds;
        ChunkGridVisibility& m_gridVisibility;
        MarchingCubesSSBOs& m_ssbos;
        GLsizei m_solidsDrawCount{ 0 };
        GLsizei m_liquidsDrawCount{ 0 };

    };
}

#endif
