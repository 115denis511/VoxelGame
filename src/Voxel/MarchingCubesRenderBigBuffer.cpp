#include "MarchingCubesRenderBigBuffer.h"

engine::MarchingCubesRenderBigBuffer::MarchingCubesRenderBigBuffer(
    ChunkGrid &grid, 
    ChunkGridBounds &gridBounds, 
    ChunkGridVisibility &gridVisibility, 
    MarchingCubesSSBOs &ssbos
) :
    MarchingCubesRenderBase(
        ChunkGridBounds::CHUNK_COUNT,
        "Shader/marchingCubesAllInOne.vert",
        "Shader/marchingCubes.frag",
        "Shader/marchingCubesLiquidBigBuffer.vert",
        "Shader/marchingCubesLiquid.frag"
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

void engine::MarchingCubesRenderBigBuffer::drawSolid(const CameraVars &cameraVars, Frustum frustum, MarchingCubes& marchingCubes) {
    m_textures.use();
    
    m_ssbos.globalChunkGridsSSBO.bind(SSBO_BLOCK__GLOBAL_CHUNK_GRIDS_STORAGE);
    m_ssbos.globalChunkSSBO.bind(SSBO_BLOCK__GLOBAL_CHUNK_STORAGE);
    m_ssbos.chunkPositionsSSBO.bind(SSBO_BLOCK__CHUNK_POSITIONS);
    marchingCubes.bindSSBO(SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS);
    m_ssbos.drawIdToDataSSBO.bind(SSBO_BLOCK__DRAW_ID_TO_CHUNK);
    
    m_gridVisibility.clearResults();
    m_gridVisibility.checkVisibility(
        cameraVars.cameraPosition, cameraVars.cameraTarget, frustum, ChunkGridVisibility::VisabilityType::CAMERA, m_grid
    );
    
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);
    GLsizei solidsDrawCount = 0;
    GLsizei liquidsDrawCount = 0;
    unsigned int gridWidth = m_gridBounds.usedChunkGridWidth;
    for (size_t z = 0; z < gridWidth; z++){
        for (size_t x = 0; x < gridWidth; x++){
            for (size_t y = 0; y < m_gridBounds.CHUNK_MAX_Y_SIZE; y++){
                VoxelChunk& chunk = m_grid.getChunk(x, y, z);
                
                if (chunk.getSolidsDrawCommandsCount() == 0 || !m_grid.isHaveChunk(x, y, z)) continue;
                if (!m_gridVisibility.isVisible(x, y, z, ChunkGridVisibility::VisabilityType::CAMERA)) continue;
                
                auto& chunkDrawCommands = chunk.getSolidsDrawCommands();
                GLsizei chunkSolidsDrawCount = chunk.getSolidsDrawCommandsCount();
                for (int i = 0; i < chunkSolidsDrawCount; i++) {
                    m_solidsDrawCommands[solidsDrawCount + i] = chunkDrawCommands[i];
                    GLuint chunkId = chunk.getIdInSSBO();
                    m_solidsDrawBufferRefs[solidsDrawCount + i] = chunkId;
                }
                solidsDrawCount += chunkSolidsDrawCount;
                
                auto& chunkLiquidDrawCommands = chunk.getLiquidsDrawCommands();
                GLsizei chunkLiquidsDrawCount = chunk.getLiquidsDrawCommandsCount();
                for (int i = 0; i < chunkLiquidsDrawCount; i++) {
                    m_liquidsDrawCommands[liquidsDrawCount + i] = chunkLiquidDrawCommands[i];
                    GLuint chunkId = chunk.getIdInSSBO();
                    m_liquidsDrawBufferRefs[liquidsDrawCount + i] = chunkId;
                }
                liquidsDrawCount += chunkLiquidsDrawCount;
            }
        }
    }

    m_shaderSolids->use();
    int commandBufferSize = solidsDrawCount * sizeof(DrawArraysIndirectCommand);
    glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &m_solidsDrawCommands[0]);
    m_ssbos.drawIdToDataSSBO.pushData(&m_solidsDrawBufferRefs[0], solidsDrawCount);
    marchingCubes.draw(solidsDrawCount);
    
    m_shaderLiquids->use();
    commandBufferSize = liquidsDrawCount * sizeof(DrawArraysIndirectCommand);
    glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &m_liquidsDrawCommands[0]);
    m_ssbos.drawIdToDataSSBO.pushData(&m_liquidsDrawBufferRefs[0], liquidsDrawCount);
    marchingCubes.draw(liquidsDrawCount);
}
