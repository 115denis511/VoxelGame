#include "MarchingCubesRenderSmallBuffers.h"

engine::MarchingCubesRenderSmallBuffers::MarchingCubesRenderSmallBuffers(
    ChunkGrid &grid, 
    ChunkGridBounds &gridBounds, 
    ChunkGridVisibility &gridVisibility, 
    MarchingCubesSSBOs &ssbos
) :
    MarchingCubesRenderBase(
        CHUNK_BATCH_MAX_SIZE,
        "Shader/marchingCubes.vert",
        "Shader/marchingCubes.frag",
        "Shader/marchingCubesLiquidSmallBuffers.vert",
        "Shader/marchingCubesLiquid.frag"
    ), 
    m_grid(grid), 
    m_gridBounds(gridBounds), 
    m_gridVisibility(gridVisibility), 
    m_ssbos(ssbos) 
{
    m_solidsDrawChunkPositions.resize(CHUNK_BATCH_MAX_SIZE);
    m_liquidsDrawChunkPositions.resize(CHUNK_BATCH_MAX_SIZE);
    m_solidsChunksToDraw.resize(CHUNK_BATCH_MAX_SIZE);
    m_liquidsChunksToDraw.resize(CHUNK_BATCH_MAX_SIZE);
}

void engine::MarchingCubesRenderSmallBuffers::initSSBOs(MarchingCubesSSBOs &ssbos) {
    m_ssbos.drawIdToDataSSBO.init(254 * CHUNK_BATCH_MAX_SIZE, BufferUsage::DYNAMIC_DRAW);
}

void engine::MarchingCubesRenderSmallBuffers::drawSolid(const CameraVars &cameraVars, Frustum frustum, MarchingCubes &marchingCubes) {
    m_textures.use();

    marchingCubes.bindSSBO(SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS);
    m_ssbos.drawIdToDataSSBO.bind(SSBO_BLOCK__DRAW_ID_TO_CHUNK);

    m_gridVisibility.clearResults();
    m_gridVisibility.checkVisibility(
        cameraVars.cameraPosition, cameraVars.cameraTarget, frustum, ChunkGridVisibility::VisabilityType::CAMERA, m_grid
    );

    VoxelPositionConverter& converter = m_grid.getPositionConverter();
    unsigned int gridWidth = m_gridBounds.usedChunkGridWidth;

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);
    m_solidsBufferIndex = 0;
    m_liquidsBufferIndex = 0;
    GLsizei solidsDrawCount = 0;
    GLsizei liquidsDrawCount = 0;
    for (size_t z = 0; z < gridWidth; z++){
        for (size_t x = 0; x < gridWidth; x++){
            for (size_t y = 0; y < m_gridBounds.CHUNK_MAX_Y_SIZE; y++){
                VoxelChunk& chunk = m_grid.getChunk(x, y, z);

                if (!m_grid.isHaveChunk(x, y, z)) continue;
                if (!m_gridVisibility.isVisible(x, y, z, ChunkGridVisibility::VisabilityType::CAMERA)) continue;

                glm::ivec2 worldChunkPos = converter.localChunkToWorldChunkPosition(x, z, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
                unsigned int height = y * m_gridBounds.CHUNCK_DIMENSION_SIZE;
                glm::vec4 localPos(
                    worldChunkPos.x * m_gridBounds.CHUNCK_DIMENSION_SIZE, 
                    height, 
                    worldChunkPos.y * m_gridBounds.CHUNCK_DIMENSION_SIZE,
                    0
                );
                
                if (chunk.getSolidsDrawCommandsCount() > 0) {
                    m_solidsDrawChunkPositions[m_solidsBufferIndex] = localPos;
    
                    auto& chunkDrawCommands = chunk.getSolidsDrawCommands();
                    GLsizei chunkDrawCount = chunk.getSolidsDrawCommandsCount();
                    for (int i = 0; i < chunkDrawCount; i++) {
                        m_solidsDrawCommands[solidsDrawCount + i] = chunkDrawCommands[i];
                        m_solidsDrawBufferRefs[solidsDrawCount + i] = m_solidsBufferIndex;
                    }
                    m_solidsChunksToDraw[m_solidsBufferIndex] = &chunk;
                    m_solidsBufferIndex++;
                    solidsDrawCount += chunkDrawCount;

                    if (m_solidsBufferIndex >= CHUNK_BATCH_MAX_SIZE) {
                        drawSolidsAccumulatedBatches(marchingCubes, solidsDrawCount);
    
                        m_solidsBufferIndex = 0;
                        solidsDrawCount = 0;
                    }
                }

                if (chunk.getLiquidsDrawCommandsCount() > 0) {
                    m_liquidsDrawChunkPositions[m_liquidsBufferIndex] = localPos;

                    auto& chunkDrawCommands = chunk.getLiquidsDrawCommands();
                    GLsizei chunkDrawCount = chunk.getLiquidsDrawCommandsCount();
                    for (int i = 0; i < chunkDrawCount; i++) {
                        m_liquidsDrawCommands[liquidsDrawCount + i] = chunkDrawCommands[i];
                        m_liquidsDrawBufferRefs[liquidsDrawCount + i] = m_liquidsBufferIndex;
                    }
                    m_liquidsChunksToDraw[m_liquidsBufferIndex] = &chunk;
                    m_liquidsBufferIndex++;
                    liquidsDrawCount += chunkDrawCount;

                    if (m_liquidsBufferIndex >= CHUNK_BATCH_MAX_SIZE) {
                        drawLiquidsAccumulatedBatches(marchingCubes, liquidsDrawCount);
    
                        m_liquidsBufferIndex = 0;
                        liquidsDrawCount = 0;
                    }
                }
            }
        }
    }
    drawSolidsAccumulatedBatches(marchingCubes, solidsDrawCount);
    drawLiquidsAccumulatedBatches(marchingCubes, liquidsDrawCount);
}

void engine::MarchingCubesRenderSmallBuffers::drawSolidsAccumulatedBatches(MarchingCubes &marchingCubes, GLsizei solidsDrawCount) {
    assert(m_solidsBufferIndex <= CHUNK_BATCH_MAX_SIZE);
    for (GLuint i = 0; i < m_solidsBufferIndex; i++) {
        assert(m_solidsChunksToDraw[i] != nullptr);
        m_solidsChunksToDraw[i]->bindCubesToDrawSSBO(i * 2);
        m_solidsChunksToDraw[i]->bindVoxelGridSSBO(i * 2 + 1);
        m_solidsChunksToDraw[i] = nullptr;
    }

    int commandBufferSize = solidsDrawCount * sizeof(DrawArraysIndirectCommand);
    glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &m_solidsDrawCommands[0]);

    m_ssbos.drawIdToDataSSBO.pushData(&m_solidsDrawBufferRefs[0], solidsDrawCount);
    UniformManager::setChunkPositions(m_solidsDrawChunkPositions);

    m_shaderSolids->use();
    marchingCubes.draw(solidsDrawCount);
}

void engine::MarchingCubesRenderSmallBuffers::drawLiquidsAccumulatedBatches(MarchingCubes &marchingCubes, GLsizei liquidsDrawCount) {
    assert(m_liquidsBufferIndex <= CHUNK_BATCH_MAX_SIZE);
    for (GLuint i = 0; i < m_liquidsBufferIndex; i++) {
        assert(m_liquidsChunksToDraw[i] != nullptr);
        m_liquidsChunksToDraw[i]->bindCubesToDrawSSBO(i * 2);
        m_liquidsChunksToDraw[i]->bindVoxelGridSSBO(i * 2 + 1);
        m_liquidsChunksToDraw[i] = nullptr;
    }

    int commandBufferSize = liquidsDrawCount * sizeof(DrawArraysIndirectCommand);
    glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &m_liquidsDrawCommands[0]);

    m_ssbos.drawIdToDataSSBO.pushData(&m_liquidsDrawBufferRefs[0], liquidsDrawCount);
    UniformManager::setChunkPositions(m_liquidsDrawChunkPositions);

    m_shaderLiquids->use();
    marchingCubes.draw(liquidsDrawCount);
};