#include "MarchingCubesRenderBigBuffer.h"

engine::MarchingCubesRenderBigBuffer::MarchingCubesRenderBigBuffer(
    ChunkGrid &grid, 
    ChunkGridBounds &gridBounds, 
    ChunkGridVisibility &gridVisibility, 
    MarchingCubesSSBOs &ssbos
) :
    MarchingCubesRenderBase(
        ChunkGridBounds::CHUNK_COUNT,
        "Shader/mc_solid_bigBuffer.vert",
        "Shader/mc_solid.frag",
        "Shader/mc_liquid_bigBuffer.vert",
        "Shader/mc_liquid.frag"
    ), 
    m_grid(grid), 
    m_gridBounds(gridBounds), 
    m_gridVisibility(gridVisibility), 
    m_ssbos(ssbos) 
{}

void engine::MarchingCubesRenderBigBuffer::initSSBOs(MarchingCubesSSBOs &ssbos) {
    ssbos.chunkPositionsSSBO.init(ChunkGridBounds::CHUNK_COUNT, BufferUsage::DYNAMIC_DRAW);
    ssbos.globalChunkSSBO.init(VoxelChunk::MARCHING_CUBES_COUNT * ChunkGridBounds::CHUNK_COUNT, BufferUsage::DYNAMIC_DRAW);
    ssbos.globalChunkGridsSSBO.init(VoxelChunk::GRID_VOXEL_COUNT * ChunkGridBounds::CHUNK_COUNT, BufferUsage::DYNAMIC_DRAW);
    ssbos.drawIdToDataSSBO.init(254 * ChunkGridBounds::CHUNK_COUNT, BufferUsage::DYNAMIC_DRAW);
}

void engine::MarchingCubesRenderBigBuffer::prepareToDraw(const CameraVars& cameraVars, Frustum frustum) {
    m_gridVisibility.clearResults();
    m_gridVisibility.checkVisibility(
        cameraVars.cameraPosition, cameraVars.cameraTarget, frustum, ChunkGridVisibility::VisabilityType::CAMERA, m_grid
    );

    m_solidsDrawCount = 0;
    m_liquidsDrawCount = 0;

    unsigned int gridWidth = m_gridBounds.usedChunkGridWidth;
    for (size_t z = 0; z < gridWidth; z++){
        for (size_t x = 0; x < gridWidth; x++){
            for (size_t y = 0; y < m_gridBounds.CHUNK_MAX_Y_SIZE; y++){
                VoxelChunk& chunk = m_grid.getChunk(x, y, z);
                
                if (!m_grid.isHaveChunk(x, y, z)) continue;
                if (!m_gridVisibility.isVisible(x, y, z, ChunkGridVisibility::VisabilityType::CAMERA)) continue;
                
                if (chunk.getSolidsDrawCommandsCount() != 0) {
                    auto& chunkDrawCommands = chunk.getSolidsDrawCommands();
                    GLsizei chunkSolidsDrawCount = chunk.getSolidsDrawCommandsCount();
                    for (int i = 0; i < chunkSolidsDrawCount; i++) {
                        m_solidsDrawCommands[m_solidsDrawCount + i] = chunkDrawCommands[i];
                        GLuint chunkId = chunk.getIdInSSBO();
                        m_solidsDrawBufferRefs[m_solidsDrawCount + i] = chunkId;
                    }
                    m_solidsDrawCount += chunkSolidsDrawCount;
                }
                
                if (chunk.getLiquidsDrawCommandsCount() != 0) {
                    auto& chunkLiquidDrawCommands = chunk.getLiquidsDrawCommands();
                    GLsizei chunkLiquidsDrawCount = chunk.getLiquidsDrawCommandsCount();
                    for (int i = 0; i < chunkLiquidsDrawCount; i++) {
                        m_liquidsDrawCommands[m_liquidsDrawCount + i] = chunkLiquidDrawCommands[i];
                        GLuint chunkId = chunk.getIdInSSBO();
                        m_liquidsDrawBufferRefs[m_liquidsDrawCount + i] = chunkId;
                    }
                    m_liquidsDrawCount += chunkLiquidsDrawCount;
                }
            }
        }
    }
}

void engine::MarchingCubesRenderBigBuffer::drawSolid(MarchingCubes& marchingCubes) {
    m_textures.use();
    
    m_ssbos.globalChunkGridsSSBO.bind(SSBO_BLOCK__GLOBAL_CHUNK_GRIDS_STORAGE);
    m_ssbos.globalChunkSSBO.bind(SSBO_BLOCK__GLOBAL_CHUNK_STORAGE);
    m_ssbos.chunkPositionsSSBO.bind(SSBO_BLOCK__CHUNK_POSITIONS);
    marchingCubes.bindSSBO(SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS);
    m_ssbos.drawIdToDataSSBO.bind(SSBO_BLOCK__DRAW_ID_TO_CHUNK);
    
    m_shaderSolids->use();
    int commandBufferSize = m_solidsDrawCount * sizeof(DrawArraysIndirectCommand);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);
    glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &m_solidsDrawCommands[0]);
    m_ssbos.drawIdToDataSSBO.pushData(&m_solidsDrawBufferRefs[0], m_solidsDrawCount);
    marchingCubes.draw(m_solidsDrawCount);
}

void engine::MarchingCubesRenderBigBuffer::drawLiquid(MarchingCubes &marchingCubes) {
    m_textures.use();
    
    m_ssbos.globalChunkGridsSSBO.bind(SSBO_BLOCK__GLOBAL_CHUNK_GRIDS_STORAGE);
    m_ssbos.globalChunkSSBO.bind(SSBO_BLOCK__GLOBAL_CHUNK_STORAGE);
    m_ssbos.chunkPositionsSSBO.bind(SSBO_BLOCK__CHUNK_POSITIONS);
    marchingCubes.bindSSBO(SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS);
    m_ssbos.drawIdToDataSSBO.bind(SSBO_BLOCK__DRAW_ID_TO_CHUNK);
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    // TODO: Прозрачные меши должны рисоваться в специальный Weighted Blended буффер прозрачности
    // https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended
    // Так же для жидкостного (прозрачного) слоя нужен отдельный текстурный массив с поддержкой альфа канала.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    m_shaderLiquids->use();
    int commandBufferSize = m_liquidsDrawCount * sizeof(DrawArraysIndirectCommand);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);
    glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &m_liquidsDrawCommands[0]);
    m_ssbos.drawIdToDataSSBO.pushData(&m_liquidsDrawBufferRefs[0], m_liquidsDrawCount);
    marchingCubes.draw(m_liquidsDrawCount);
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_BLEND);
}
