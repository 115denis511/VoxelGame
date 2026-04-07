#include "MarchingCubesRenderSmallBuffers.h"

engine::MarchingCubesRenderSmallBuffers::MarchingCubesRenderSmallBuffers(
    ChunkGrid &grid, 
    ChunkGridBounds &gridBounds, 
    ChunkGridVisibility &gridVisibility, 
    MarchingCubesSSBOs &ssbos
) :
    MarchingCubesRenderBase(
        CHUNK_BATCH_MAX_SIZE,
        "Shader/mc_solid_smallBuffers.vert",
        "Shader/mc_solid.frag",
        "Shader/mc_liquid_smallBuffers.vert",
        "Shader/mc_liquid.frag"
    ), 
    m_grid(grid), 
    m_gridBounds(gridBounds), 
    m_gridVisibility(gridVisibility), 
    m_ssbos(ssbos) 
{
    m_solidBatches.resize(BATCHES_COUNT);
    m_liquidBatches.resize(BATCHES_COUNT);
}

void engine::MarchingCubesRenderSmallBuffers::initSSBOs(MarchingCubesSSBOs &ssbos) {
    m_ssbos.drawIdToDataSSBO.init(254 * CHUNK_BATCH_MAX_SIZE, BufferUsage::DYNAMIC_DRAW);
}

void engine::MarchingCubesRenderSmallBuffers::prepareToDraw(const CameraVars &cameraVars, Frustum frustum) {
    m_gridVisibility.clearResults();
    m_gridVisibility.checkVisibility(
        cameraVars.cameraPosition, cameraVars.cameraTarget, frustum, ChunkGridVisibility::VisabilityType::CAMERA, m_grid
    );

    VoxelPositionConverter& converter = m_grid.getPositionConverter();
    unsigned int gridWidth = m_gridBounds.usedChunkGridWidth;
    
    m_solidBatches[0].clear();
    m_liquidBatches[0].clear();
    m_solidBatchesCount = 0;
    m_liquidBatchesCount = 0;
    GLuint solidBufferId = 0;
    GLuint liquidBufferId = 0;
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
                    DrawBatch& batch = m_solidBatches[m_solidBatchesCount];

                    batch.chunkPositions[solidBufferId] = localPos;
                    
                    auto& chunkDrawCommands = chunk.getSolidsDrawCommands();
                    GLsizei chunkDrawCount = chunk.getSolidsDrawCommandsCount();
                    for (int i = 0; i < chunkDrawCount; i++) {
                        batch.drawCommands[solidsDrawCount + i] = chunkDrawCommands[i];
                        batch.bufferRefs[solidsDrawCount + i] = solidBufferId;
                    }
                    batch.chunksToDraw[solidBufferId] = &chunk;
                    batch.chunkCount++;
                    batch.drawCommandsCount += chunkDrawCount;
                    solidsDrawCount += chunkDrawCount;
                    
                    solidBufferId++;
                    if (solidBufferId >= CHUNK_BATCH_MAX_SIZE) {
                        m_solidBatchesCount++;
                        
                        solidsDrawCount = 0;
                        solidBufferId = 0;

                        m_solidBatches[m_solidBatchesCount].clear();
                    }
                }

                if (chunk.getLiquidsDrawCommandsCount() > 0) {
                    DrawBatch& batch = m_liquidBatches[m_liquidBatchesCount];

                    batch.chunkPositions[liquidBufferId] = localPos;

                    auto& chunkDrawCommands = chunk.getLiquidsDrawCommands();
                    GLsizei chunkDrawCount = chunk.getLiquidsDrawCommandsCount();
                    for (int i = 0; i < chunkDrawCount; i++) {
                        batch.drawCommands[liquidsDrawCount + i] = chunkDrawCommands[i];
                        batch.bufferRefs[liquidsDrawCount + i] = liquidBufferId;
                    }
                    batch.chunksToDraw[liquidBufferId] = &chunk;
                    batch.chunkCount++;
                    batch.drawCommandsCount += chunkDrawCount;
                    liquidsDrawCount += chunkDrawCount;
                    
                    liquidBufferId++;
                    if (liquidBufferId >= CHUNK_BATCH_MAX_SIZE) {
                        m_liquidBatchesCount++;
                        
                        liquidsDrawCount = 0;
                        liquidBufferId = 0;

                        m_liquidBatches[m_liquidBatchesCount].clear();
                    }
                }
            }
        }
    }

    m_solidBatchesCount++;
    m_liquidBatchesCount++;
}

void engine::MarchingCubesRenderSmallBuffers::drawSolid(MarchingCubes &marchingCubes) {
    m_shaderSolids->use();
    m_textures.use();

    marchingCubes.bindSSBO(SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS);
    m_ssbos.drawIdToDataSSBO.bind(SSBO_BLOCK__DRAW_ID_TO_CHUNK);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);

    for (GLuint batchId = 0; batchId < m_solidBatchesCount; batchId++) {
        DrawBatch& batch = m_solidBatches[batchId];
        for (GLuint i = 0; i < batch.chunkCount; i++) {
            assert(batch.chunksToDraw[i] != nullptr);
            batch.chunksToDraw[i]->bindCubesToDrawSSBO(i * 2);
            batch.chunksToDraw[i]->bindVoxelGridSSBO(i * 2 + 1);
            batch.chunksToDraw[i] = nullptr;
        }

        m_ssbos.drawIdToDataSSBO.pushData(&batch.bufferRefs[0], batch.drawCommandsCount);
        UniformManager::setChunkPositions(batch.chunkPositions);

        int commandBufferSize = batch.drawCommandsCount * sizeof(DrawArraysIndirectCommand);
        glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &batch.drawCommands[0]);

        marchingCubes.draw(batch.drawCommandsCount);
    }
}

void engine::MarchingCubesRenderSmallBuffers::drawLiquid(MarchingCubes &marchingCubes) {
    m_shaderLiquids->use();
    m_textures.use();

    marchingCubes.bindSSBO(SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS);
    m_ssbos.drawIdToDataSSBO.bind(SSBO_BLOCK__DRAW_ID_TO_CHUNK);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);

    for (GLuint batchId = 0; batchId < m_liquidBatchesCount; batchId++) {
        DrawBatch& batch = m_liquidBatches[batchId];
        for (GLuint i = 0; i < batch.chunkCount; i++) {
            assert(batch.chunksToDraw[i] != nullptr);
            batch.chunksToDraw[i]->bindCubesToDrawSSBO(i * 2);
            batch.chunksToDraw[i]->bindVoxelGridSSBO(i * 2 + 1);
            batch.chunksToDraw[i] = nullptr;
        }

        m_ssbos.drawIdToDataSSBO.pushData(&batch.bufferRefs[0], batch.drawCommandsCount);
        UniformManager::setChunkPositions(batch.chunkPositions);

        int commandBufferSize = batch.drawCommandsCount * sizeof(DrawArraysIndirectCommand);
        glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &batch.drawCommands[0]);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        
        // TODO: Прозрачные меши должны рисоваться в специальный Weighted Blended буффер прозрачности
        // https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended
        // Так же для жидкостного (прозрачного) слоя нужен отдельный текстурный массив с поддержкой альфа канала.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
        
        marchingCubes.draw(batch.drawCommandsCount);

        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_BLEND);
    }
}
