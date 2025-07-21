#include "MarchingCubesManager.h"

engine::MarchingCubesManager* engine::MarchingCubesManager::g_instance{ nullptr };

engine::MarchingCubesManager::~MarchingCubesManager() {
    glDeleteBuffers(1, &m_commandBuffer);
}

engine::MarchingCubesManager *engine::MarchingCubesManager::getInstance()
{
    return g_instance;
}

bool engine::MarchingCubesManager::isPositionHasSolidVoxel(const glm::vec3 &position) {
    return m_grid.isPositionHasSolidVoxel(position, m_gridBounds);
}

bool engine::MarchingCubesManager::init() {
    g_instance = new MarchingCubesManager();

    return false;
}

void engine::MarchingCubesManager::freeResources() {
    delete g_instance;
}

void engine::MarchingCubesManager::updateChunks(size_t maxCount) {
    for (size_t i = 0; i < maxCount && !m_toUpdateQueue.empty(); i++) {
        VoxelChunk& chunk = popFromUpdateQueue();
        if (!chunk.isInUse()) { continue; }

        if (chunk.isMustClearOnUpdate()) { 
            chunk.clear(); 
            chunk.setMustClearOnUpdateFlag(false);
        }

        m_solver.regenerateChunk(m_marchingCubes, chunk);
    }
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
    for (size_t i = 0; i < std::size(m_chunks); i++) {
        m_freeChunkIndices.push(i);
    }

    glCreateBuffers(1, &m_commandBuffer);
    constexpr GLuint byteSize = sizeof(engine::DrawArraysIndirectCommand) * 254 * 15;
    glNamedBufferData(m_commandBuffer, byteSize, NULL, GL_DYNAMIC_DRAW);

    // 2147483647
    // Этого хватит для одного общего буффера для вокселей чанков
    //GLint size;
    //glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &size);
    //std::cout << "GL_MAX_SHADER_STORAGE_BLOCK_SIZE is " << size << " bytes.\n";
}

void engine::MarchingCubesManager::draw(Shader& shader, const CameraVars& cameraVars, Frustum frustum) {
    shader.use();
    m_textures.use();

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

                if (bufferIndex >= engine_properties::SSBO_VOXEL_VERTECES_DATA_IDS_BLOCK_ID) {
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

    ShaderStorageManager::setChunkBufferRef(m_drawBufferRefs, drawCount);
    UniformManager::setChunkPositions(m_drawChunkPositions);

    m_marchingCubes.draw(drawCount);
}

void engine::MarchingCubesManager::pushToUpdateQueue(size_t index) {
    VoxelChunk& chunk = m_chunks[index];
    if (!chunk.isInUpdateQueue()) {
        chunk.setInUpdateQueueFlag(true);
        m_toUpdateQueue.push(index);
    }
}

engine::VoxelChunk& engine::MarchingCubesManager::popFromUpdateQueue() {
    VoxelChunk& chunk = m_chunks[m_toUpdateQueue.top()];
    chunk.setInUpdateQueueFlag(false);
    m_toUpdateQueue.pop();
    return chunk;
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
    VoxelChunk& chunk = m_chunks[chunkId];
    pushToUpdateQueue(chunkId);
    chunk.setVoxel(localVoxel.x, localVoxel.y, localVoxel.z, id, size);

    if (localVoxel.x == 0) {
        glm::ivec3 chunkX = glm::ivec3(chunkPos.x - 1, chunkPos.y, chunkPos.z);
        if (m_gridBounds.isChunkInbounds(chunkX.x, chunkX.y, chunkX.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y, localXZ.y);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(m_gridBounds.CHUNCK_DIMENSION_SIZE, localVoxel.y, localVoxel.z, id, size);
        }
    }

    if (localVoxel.z == 0) {
        glm::ivec3 chunkZ = glm::ivec3(chunkPos.x, chunkPos.y, chunkPos.z - 1);
        if (m_gridBounds.isChunkInbounds(chunkZ.x, chunkZ.y, chunkZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y, localXZ.y - 1);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(localVoxel.x, localVoxel.y, m_gridBounds.CHUNCK_DIMENSION_SIZE, id, size);
        }
    }

    if (localVoxel.y == 0) {
        glm::ivec3 chunkY = glm::ivec3(chunkPos.x, chunkPos.y - 1, chunkPos.z);
        if (m_gridBounds.isChunkInbounds(chunkY.x, chunkY.y, chunkY.z)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y - 1, localXZ.y);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(localVoxel.x, m_gridBounds.CHUNCK_DIMENSION_SIZE, localVoxel.z, id, size);
        }
    }

    if (localVoxel.x == 0 && localVoxel.z == 0) {
        glm::ivec3 chunkXZ = glm::ivec3(chunkPos.x - 1, chunkPos.y, chunkPos.z - 1);
        if (m_gridBounds.isChunkInbounds(chunkXZ.x, chunkXZ.y, chunkXZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y, localXZ.y - 1);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(m_gridBounds.CHUNCK_DIMENSION_SIZE, localVoxel.y, m_gridBounds.CHUNCK_DIMENSION_SIZE, id, size);
        }
    }

    if (localVoxel.x == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkXY = glm::ivec3(chunkPos.x - 1, chunkPos.y - 1, chunkPos.z);
        if (m_gridBounds.isChunkInbounds(chunkXY.x, chunkXY.y, chunkXY.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y - 1, localXZ.y);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(m_gridBounds.CHUNCK_DIMENSION_SIZE, m_gridBounds.CHUNCK_DIMENSION_SIZE, localVoxel.z, id, size);
        }
    }

    if (localVoxel.z == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkYZ = glm::ivec3(chunkPos.x, chunkPos.y - 1, chunkPos.z - 1);
        if (m_gridBounds.isChunkInbounds(chunkYZ.x, chunkYZ.y, chunkYZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y - 1, localXZ.y - 1);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(localVoxel.x, m_gridBounds.CHUNCK_DIMENSION_SIZE, m_gridBounds.CHUNCK_DIMENSION_SIZE, id, size);
        }
    }

    if (localVoxel.x == 0 && localVoxel.z == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkXYZ = glm::ivec3(chunkPos.x - 1, chunkPos.y - 1, chunkPos.z - 1);
        if (m_gridBounds.isChunkInbounds(chunkXYZ.x, chunkXYZ.y, chunkXYZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y - 1, localXZ.y - 1);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(m_gridBounds.CHUNCK_DIMENSION_SIZE, m_gridBounds.CHUNCK_DIMENSION_SIZE, m_gridBounds.CHUNCK_DIMENSION_SIZE, id, size);
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
    resizeChunkGrid(uRadius * 2);

    m_gridBounds.currentOriginChunk = glm::ivec2(m_gridBounds.currentCenterChunk.x - radius, m_gridBounds.currentCenterChunk.y - radius);
}

void engine::MarchingCubesManager::resizeChunkGrid(unsigned int size) {
    if (size % 2 == 1) size -= 1;
    if (size > m_gridBounds.CHUNK_MAX_X_Z_SIZE) size = m_gridBounds.CHUNK_MAX_X_Z_SIZE;
    unsigned int usedChunkDistance = m_gridBounds.usedChunkGridWidth;
    if (usedChunkDistance == size) return;

    if (size < usedChunkDistance) {
        std::vector<int> chunksToDelete;
        m_grid.resizeToSmaller(size, m_gridBounds, chunksToDelete); 

        for (int id : chunksToDelete) {
            m_freeChunkIndices.push(id);
            m_chunks[id].setInUseFlag(false);
        }
    }
    else {
        m_toGenerateQueue.clear();
        m_grid.resizeToBigger(size, m_gridBounds, m_toGenerateQueue);

        for (glm::ivec2& pos : m_toGenerateQueue) {
            for (int y = 0; y < m_gridBounds.CHUNK_MAX_Y_SIZE; y++) {
                size_t id = m_freeChunkIndices.top();
                m_freeChunkIndices.pop();

                VoxelChunk& chunk = m_chunks[id];
                chunk.setInUseFlag(true);
                m_grid.setChunk(pos.x, y, pos.y, id);

                // TODO: Переместить код генерации мира в updateChunks.
                // Затем убрать очистку чанка отсюда(убрать следующую строку и раскоментировать последующую).
                chunk.clear();
                //chunk.setMustClearOnUpdateFlag(true);

                // Временная "прозрачность" видимости чанка.
                // Пока чанк находится в очереди на перегенерацию, оставшиеся старые состояния видимости
                // могут блокировать видимость через пустой чанк.
                chunk.updateVisibilityStatesForEmptyChunk();

                // ХАК: Чистим количество рисований, чтобы чанк ожидающий перегенерации
                // при отрисовке он не рендерил старый необновленный чанк.
                // Таким образом обходится надобность чистить чанк сейчас, так как очистка
                // переносится в метод обновления чанка, который может работать в отдельном потоке.
                m_chunks[id].clearDrawCommands();

                m_toUpdateQueue.push(id);
                chunk.setInUpdateQueueFlag(true);

                // Временный код генерации мира
                if (y == 0) {
                    for (size_t x = 0; x < 32; x++) {
                        for (size_t z = 0; z < 32; z++){
                            m_chunks[id].setVoxel(x,1,z, 0);
                        }
                    }

                    if (pos.x <= 0 && std::abs(pos.x) <= std::abs(y)) {
                        for (size_t y = 0; y < 32; y++) {
                            for (size_t z = 0; z < 32; z++){
                                m_chunks[id].setVoxel(0,y,z, 1);
                                m_chunks[id].setVoxel(31,y,z, 1);
                                m_chunks[id].setVoxel(z,y,0, 1);
                                m_chunks[id].setVoxel(z,y,31, 1);
                                m_chunks[id].setVoxel(z,0,y, 1);
                                m_chunks[id].setVoxel(z,31,y, 1);
                            }
                        }
                    }
                }

            }
        }
        m_toGenerateQueue.clear();
    }
}
