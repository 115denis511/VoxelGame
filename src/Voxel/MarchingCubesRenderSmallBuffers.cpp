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
        "PLACEHOLDER",
        "Shader/marchingCubesLiquid.frag"
    ), 
    m_grid(grid), 
    m_gridBounds(gridBounds), 
    m_gridVisibility(gridVisibility), 
    m_ssbos(ssbos) 
{
    m_drawChunkPositions.resize(CHUNK_BATCH_MAX_SIZE);
}

void engine::MarchingCubesRenderSmallBuffers::initSSBOs(MarchingCubesSSBOs &ssbos) {
    m_ssbos.drawIdToDataSSBO.init(254 * CHUNK_BATCH_MAX_SIZE, BufferUsage::DYNAMIC_DRAW);
}

void engine::MarchingCubesRenderSmallBuffers::drawSolid(const CameraVars &cameraVars, Frustum frustum, MarchingCubes &marchingCubes) {
    m_shaderSolids->use();
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
    GLuint bufferIndex = 0;
    GLsizei drawCount = 0;
    for (size_t z = 0; z < gridWidth; z++){
        for (size_t x = 0; x < gridWidth; x++){
            for (size_t y = 0; y < m_gridBounds.CHUNK_MAX_Y_SIZE; y++){
                VoxelChunk& chunk = m_grid.getChunk(x, y, z);

                if (chunk.getDrawCommandsCount() == 0 || !m_grid.isHaveChunk(x, y, z)) continue;
                if (!m_gridVisibility.isVisible(x, y, z, ChunkGridVisibility::VisabilityType::CAMERA)) continue;

                glm::ivec2 worldChunkPos = converter.localChunkToWorldChunkPosition(x, z, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
                unsigned int height = y * m_gridBounds.CHUNCK_DIMENSION_SIZE;
                m_drawChunkPositions[bufferIndex] = glm::vec4(
                    worldChunkPos.x * m_gridBounds.CHUNCK_DIMENSION_SIZE, 
                    height, 
                    worldChunkPos.y * m_gridBounds.CHUNCK_DIMENSION_SIZE,
                    0
                );

                auto& chunkDrawCommands = chunk.getDrawCommands();
                GLsizei chunkDrawCount = chunk.getDrawCommandsCount();
                for (int i = 0; i < chunkDrawCount; i++) {
                    m_solidsDrawCommands[drawCount + i] = chunkDrawCommands[i];
                    m_solidsDrawBufferRefs[drawCount + i] = bufferIndex;
                }
                chunk.bindCubesToDrawSSBO(bufferIndex * 2);
                chunk.bindVoxelGridSSBO(bufferIndex * 2 + 1);
                bufferIndex++;
                drawCount += chunkDrawCount;

                if (bufferIndex >= CHUNK_BATCH_MAX_SIZE) {
                    drawAccumulatedBatches(marchingCubes, drawCount);

                    bufferIndex = 0;
                    drawCount = 0;
                }
            }
        }
    }
    drawAccumulatedBatches(marchingCubes, drawCount);
}

void engine::MarchingCubesRenderSmallBuffers::drawAccumulatedBatches(MarchingCubes &marchingCubes, GLsizei drawCount) {
    int commandBufferSize = drawCount * sizeof(DrawArraysIndirectCommand);
    glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &m_solidsDrawCommands[0]);

    m_ssbos.drawIdToDataSSBO.pushData(&m_solidsDrawBufferRefs[0], drawCount);
    UniformManager::setChunkPositions(m_drawChunkPositions);

    marchingCubes.draw(drawCount);
};