#include "MarchingCubesManager.h"

engine::MarchingCubesManager* engine::MarchingCubesManager::g_instance{ nullptr };

engine::MarchingCubesManager::~MarchingCubesManager() {
    glDeleteBuffers(1, &m_commandBuffer);

    delete m_shader;
}

engine::MarchingCubesManager *engine::MarchingCubesManager::getInstance() {
    return g_instance;
}

bool engine::MarchingCubesManager::isPositionHasSolidVoxel(const glm::vec3 &position) {
    return m_grid.isPositionHasSolidVoxel(position);
}

bool engine::MarchingCubesManager::init() {
    g_instance = new MarchingCubesManager();

    return false;
}

void engine::MarchingCubesManager::freeResources() {
    delete g_instance;
}

void engine::MarchingCubesManager::updateChunks(size_t maxCount) {
    m_gridChanger.generateChunks(m_gridBounds, m_converter, m_usingGlobalChunkSSBO, 1);
    m_gridChanger.updateChunks(m_marchingCubes, m_globalChunkSSBO, 8);
}

bool engine::MarchingCubesManager::setVoxelTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents) {
    return m_textures.setTexture(layer, rawImage, width, height, nrComponents);
}

bool engine::MarchingCubesManager::setVoxelTexture(int layer, std::string path) {
    return m_textures.setTexture(layer, path);
}

bool engine::MarchingCubesManager::setVoxelTexture(int layer, glm::vec4 color) {
    return m_textures.setTexture(layer, color);
}

void engine::MarchingCubesManager::startTextureEditing() {
    if (m_textures.isResident())
        m_textures.makeNonResident();
}

void engine::MarchingCubesManager::endTextureEditing() {
    if (!m_textures.isResident()) {
        m_textures.updateMipmap();
        m_textures.makeResident();
    }
}

engine::MarchingCubesManager::MarchingCubesManager() {
    GLint size;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &size);
    constexpr GLsizeiptr CHUNK_BUFFER_SIZE = VoxelChunk::VOXEL_CHUNK_BYTE_SIZE * ChunkGridBounds::CHUNK_COUNT;

    if (size < CHUNK_BUFFER_SIZE) { 
        m_usingGlobalChunkSSBO = false;

        m_drawCommands.resize(254 * CHUNK_BATCH_MAX_SIZE);
        m_drawBufferRefs.resize(254 * CHUNK_BATCH_MAX_SIZE);
        m_drawChunkPositions.resize(CHUNK_BATCH_MAX_SIZE);

        m_drawIdToDataSSBO.init(254 * CHUNK_BATCH_MAX_SIZE, BufferUsage::DYNAMIC_DRAW);

        m_shader = new Shader("Shader/marchingCubes.vert", "Shader/marchingCubes.frag");

        glCreateBuffers(1, &m_commandBuffer);
        constexpr GLuint byteSize = sizeof(engine::DrawArraysIndirectCommand) * 254 * CHUNK_BATCH_MAX_SIZE;
        glNamedBufferData(m_commandBuffer, byteSize, NULL, GL_DYNAMIC_DRAW);

        m_grid.initChunksSSBO();
    }
    else {
        m_usingGlobalChunkSSBO = true;

        m_drawCommands.resize(254 * ChunkGridBounds::CHUNK_COUNT);
        m_drawBufferRefs.resize(254 * ChunkGridBounds::CHUNK_COUNT);
        
        m_chunkPositionsSSBO.init(ChunkGridBounds::CHUNK_COUNT, BufferUsage::DYNAMIC_DRAW);
        m_globalChunkSSBO.init(31 * 31 * 31 * ChunkGridBounds::CHUNK_COUNT, BufferUsage::DYNAMIC_DRAW);
        m_drawIdToDataSSBO.init(254 * ChunkGridBounds::CHUNK_COUNT, BufferUsage::DYNAMIC_DRAW);

        m_shader = new Shader("Shader/marchingCubesAllInOne.vert", "Shader/marchingCubes.frag");
        
        glCreateBuffers(1, &m_commandBuffer);
        constexpr GLuint byteSize = sizeof(engine::DrawArraysIndirectCommand) * 254 * ChunkGridBounds::CHUNK_COUNT;
        glNamedBufferData(m_commandBuffer, byteSize, NULL, GL_DYNAMIC_DRAW);

        m_grid.initChunkLocationsInSSBO();
    }
}

void engine::MarchingCubesManager::draw(const CameraVars &cameraVars, Frustum frustum) {
    if (m_usingGlobalChunkSSBO) {
        drawAllInOne(cameraVars, frustum);
    }
    else {
        drawBatches(cameraVars, frustum);
    }
}

void engine::MarchingCubesManager::drawAllInOne(const CameraVars &cameraVars, Frustum frustum) {
    m_shader->use();
    m_textures.use();

    m_globalChunkSSBO.bind(SSBO_BLOCK__GLOBAL_CHUNK_STOTAGE);
    m_chunkPositionsSSBO.bind(SSBO_BLOCK__CHUNK_POSITIONS);
    m_marchingCubes.bindSSBO(SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS);
    m_drawIdToDataSSBO.bind(SSBO_BLOCK__DRAW_ID_TO_CHUNK);

    m_gridVisibility.clearResults();
    m_gridVisibility.checkVisibility(
        cameraVars.cameraPosition, cameraVars.cameraTarget, frustum, ChunkGridVisibility::VisabilityType::CAMERA, m_grid, m_gridBounds
    );

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);
    GLsizei drawCount = 0;
    unsigned int gridWidth = m_gridBounds.usedChunkGridWidth;
    for (size_t z = 0; z < gridWidth; z++){
        for (size_t x = 0; x < gridWidth; x++){
            for (size_t y = 0; y < m_gridBounds.CHUNK_MAX_Y_SIZE; y++){
                VoxelChunk& chunk = m_grid.getChunk(x, y, z);

                if (chunk.getDrawCommandsCount() == 0 || !m_grid.isInUse(x, y, z)) continue;
                if (!m_gridVisibility.isVisible(x, y, z, ChunkGridVisibility::VisabilityType::CAMERA)) continue;

                auto& chunkDrawCommands = chunk.getDrawCommands();
                GLsizei chunkDrawCount = chunk.getDrawCommandsCount();
                for (int i = 0; i < chunkDrawCount; i++) {
                    m_drawCommands[drawCount + i] = chunkDrawCommands[i];
                    GLuint chunkId = chunk.getIdInSSBO();
                    m_drawBufferRefs[drawCount + i] = chunkId;
                }
                drawCount += chunkDrawCount;
            }
        }
    }
    int commandBufferSize = drawCount * sizeof(DrawArraysIndirectCommand);
    glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &m_drawCommands[0]);
    m_drawIdToDataSSBO.pushData(&m_drawBufferRefs[0], drawCount);
    m_marchingCubes.draw(drawCount);
}

void engine::MarchingCubesManager::drawBatches(const CameraVars& cameraVars, Frustum frustum) {
    m_shader->use();
    m_textures.use();

    m_marchingCubes.bindSSBO(SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS);
    m_drawIdToDataSSBO.bind(SSBO_BLOCK__DRAW_ID_TO_CHUNK);

    //glm::vec3 debugCameraPos = cameraVars.cameraPosition; debugCameraPos.y = 1.f;
    m_gridVisibility.clearResults();
    m_gridVisibility.checkVisibility(
        cameraVars.cameraPosition, cameraVars.cameraTarget, frustum, ChunkGridVisibility::VisabilityType::CAMERA, m_grid, m_gridBounds
    );
    /*m_gridVisibility.checkVisibility(
        debugCameraPos, cameraVars.cameraTarget, frustum, ChunkGridVisibility::VisabilityType::CAMERA, m_grid, m_gridBounds
    );*/

    unsigned int gridWidth = m_gridBounds.usedChunkGridWidth;

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_commandBuffer);
    GLuint bufferIndex = 0;
    GLsizei drawCount = 0;
    for (size_t z = 0; z < gridWidth; z++){
        for (size_t x = 0; x < gridWidth; x++){
            for (size_t y = 0; y < m_gridBounds.CHUNK_MAX_Y_SIZE; y++){
                VoxelChunk& chunk = m_grid.getChunk(x, y, z);

                if (chunk.getDrawCommandsCount() == 0) continue;
                if (!m_gridVisibility.isVisible(x, y, z, ChunkGridVisibility::VisabilityType::CAMERA)) continue;

                glm::ivec2 worldChunkPos = m_converter.localChunkToWorldChunkPosition(x, z, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
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
                    m_drawCommands[drawCount + i] = chunkDrawCommands[i];
                    m_drawBufferRefs[drawCount + i] = bufferIndex;
                }
                chunk.bindSSBO(bufferIndex);
                bufferIndex++;
                drawCount += chunkDrawCount;

                if (bufferIndex >= CHUNK_BATCH_MAX_SIZE) {
                    drawAccumulatedBatches(drawCount);

                    bufferIndex = 0;
                    drawCount = 0;
                }
            }
        }
    }
    drawAccumulatedBatches(drawCount);
}

void engine::MarchingCubesManager::drawAccumulatedBatches(GLsizei drawCount) {
    int commandBufferSize = drawCount * sizeof(DrawArraysIndirectCommand);
    glNamedBufferSubData(m_commandBuffer, 0, commandBufferSize, &m_drawCommands[0]);

    m_drawIdToDataSSBO.pushData(&m_drawBufferRefs[0], drawCount);
    UniformManager::setChunkPositions(m_drawChunkPositions);

    m_marchingCubes.draw(drawCount);
}

bool engine::MarchingCubesManager::raycastVoxel(const glm::vec3& position, const glm::vec3& direction, float maxDistance, glm::ivec3& hitPosition, glm::ivec3& hitFace) {
    glm::ivec3 voxelPosition = VoxelPositionConverter::worldPositionToVoxelPosition(position, m_gridBounds.VOXEL_SIZE);
    return VoxelRaycast::raycastVoxel(m_grid, m_gridBounds, voxelPosition, direction, maxDistance, hitPosition, hitFace);
}

void engine::MarchingCubesManager::setVoxel(const glm::vec3 &position, uint8_t id, uint8_t size) {
    glm::ivec3 chunkPos = m_converter.worldPositionToChunkPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);
    if (!m_gridBounds.isChunkInbounds(chunkPos.x, chunkPos.y, chunkPos.z)) return;
    glm::ivec2 localXZ = m_converter.worldChunkToLocalChunkPosition(chunkPos.x, chunkPos.z, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
    glm::ivec3 localVoxel = m_converter.worldPositionToLocalVoxelPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);

    size_t chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y, localXZ.y);
    VoxelChunk& chunk = m_grid.getChunk(chunkId);
    m_gridChanger.pushToUpdateQueue(chunkId);
    chunk.setVoxel(localVoxel.x, localVoxel.y, localVoxel.z, id, size);

    if (localVoxel.x == 0) {
        glm::ivec3 chunkX = glm::ivec3(chunkPos.x - 1, chunkPos.y, chunkPos.z);
        if (m_gridBounds.isChunkInbounds(chunkX.x, chunkX.y, chunkX.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y, localXZ.y);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& chunk = m_grid.getChunk(chunkId);
            chunk.setVoxel(m_gridBounds.CHUNCK_DIMENSION_SIZE, localVoxel.y, localVoxel.z, id, size);
        }
    }

    if (localVoxel.z == 0) {
        glm::ivec3 chunkZ = glm::ivec3(chunkPos.x, chunkPos.y, chunkPos.z - 1);
        if (m_gridBounds.isChunkInbounds(chunkZ.x, chunkZ.y, chunkZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y, localXZ.y - 1);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& chunk = m_grid.getChunk(chunkId);
            chunk.setVoxel(localVoxel.x, localVoxel.y, m_gridBounds.CHUNCK_DIMENSION_SIZE, id, size);
        }
    }

    if (localVoxel.y == 0) {
        glm::ivec3 chunkY = glm::ivec3(chunkPos.x, chunkPos.y - 1, chunkPos.z);
        if (m_gridBounds.isChunkInbounds(chunkY.x, chunkY.y, chunkY.z)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y - 1, localXZ.y);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& chunk = m_grid.getChunk(chunkId);
            chunk.setVoxel(localVoxel.x, m_gridBounds.CHUNCK_DIMENSION_SIZE, localVoxel.z, id, size);
        }
    }

    if (localVoxel.x == 0 && localVoxel.z == 0) {
        glm::ivec3 chunkXZ = glm::ivec3(chunkPos.x - 1, chunkPos.y, chunkPos.z - 1);
        if (m_gridBounds.isChunkInbounds(chunkXZ.x, chunkXZ.y, chunkXZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y, localXZ.y - 1);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& chunk = m_grid.getChunk(chunkId);
            chunk.setVoxel(m_gridBounds.CHUNCK_DIMENSION_SIZE, localVoxel.y, m_gridBounds.CHUNCK_DIMENSION_SIZE, id, size);
        }
    }

    if (localVoxel.x == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkXY = glm::ivec3(chunkPos.x - 1, chunkPos.y - 1, chunkPos.z);
        if (m_gridBounds.isChunkInbounds(chunkXY.x, chunkXY.y, chunkXY.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y - 1, localXZ.y);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& chunk = m_grid.getChunk(chunkId);
            chunk.setVoxel(m_gridBounds.CHUNCK_DIMENSION_SIZE, m_gridBounds.CHUNCK_DIMENSION_SIZE, localVoxel.z, id, size);
        }
    }

    if (localVoxel.z == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkYZ = glm::ivec3(chunkPos.x, chunkPos.y - 1, chunkPos.z - 1);
        if (m_gridBounds.isChunkInbounds(chunkYZ.x, chunkYZ.y, chunkYZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y - 1, localXZ.y - 1);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& chunk = m_grid.getChunk(chunkId);
            chunk.setVoxel(localVoxel.x, m_gridBounds.CHUNCK_DIMENSION_SIZE, m_gridBounds.CHUNCK_DIMENSION_SIZE, id, size);
        }
    }

    if (localVoxel.x == 0 && localVoxel.z == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkXYZ = glm::ivec3(chunkPos.x - 1, chunkPos.y - 1, chunkPos.z - 1);
        if (m_gridBounds.isChunkInbounds(chunkXYZ.x, chunkXYZ.y, chunkXYZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y - 1, localXZ.y - 1);
            m_gridChanger.pushToUpdateQueue(chunkId);

            VoxelChunk& chunk = m_grid.getChunk(chunkId);
            chunk.setVoxel(m_gridBounds.CHUNCK_DIMENSION_SIZE, m_gridBounds.CHUNCK_DIMENSION_SIZE, m_gridBounds.CHUNCK_DIMENSION_SIZE, id, size);
        }
    }
}

engine::MarchingCubesVoxel engine::MarchingCubesManager::getVoxel(const glm::vec3 &position) {
    glm::ivec3 chunkPos = m_converter.worldPositionToChunkPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);
    if (!m_gridBounds.isChunkInbounds(chunkPos.x, chunkPos.y, chunkPos.z)) {
        return MarchingCubesVoxel(255, 0);
    }
    glm::ivec2 localXZ = m_converter.worldChunkToLocalChunkPosition(chunkPos.x, chunkPos.z, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
    glm::ivec3 localVoxel = m_converter.worldPositionToLocalVoxelPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);

    VoxelChunk& chunk = m_grid.getChunk(localXZ.x, chunkPos.y, localXZ.y);

    return chunk.getVoxel(localVoxel.x, localVoxel.y, localVoxel.z);
}

void engine::MarchingCubesManager::setRenderChunkRadius(int radius) {
    unsigned int uRadius = (unsigned int)radius;

    if (radius <= 0) uRadius = 0;
    else if (uRadius > m_gridBounds.MAX_RENDER_CHUNK_RADIUS) uRadius = m_gridBounds.MAX_RENDER_CHUNK_RADIUS;

    if (uRadius == m_renderChunkRadius) return;

    m_renderChunkRadius = uRadius;
    m_gridBounds.currentOriginChunk = glm::ivec2(m_gridBounds.currentCenterChunk.x - radius, m_gridBounds.currentCenterChunk.y - radius);

    m_gridChanger.resizeGrid(
        m_gridBounds, 
        m_chunkPositionsSSBO, 
        m_converter, 
        m_usingGlobalChunkSSBO, 
        uRadius * 2
    );
}
