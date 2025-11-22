#ifndef __VOXEL__MARCHING_CUBES_SSBO_S_H__
#define __VOXEL__MARCHING_CUBES_SSBO_S_H__

#include "../stdafx.h"
#include "../Render/ShaderStorageManager.h"
#include "../Render/ShaderStorageBuffer.h"
#include "Voxel.h"

namespace engine {
    struct MarchingCubesSSBOs {
        ShaderStorageBuffer<glm::ivec4> chunkPositionsSSBO;
        ShaderStorageBuffer<GLuint> globalChunkSSBO;
        ShaderStorageBuffer<Voxel> globalChunkGridsSSBO;
        ShaderStorageBuffer<GLuint> drawIdToDataSSBO;
    };
}

#endif