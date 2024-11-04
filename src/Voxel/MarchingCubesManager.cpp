#include "MarchingCubesManager.h"

engine::MarchingCubesManager* engine::MarchingCubesManager::g_instance{ nullptr };

engine::MarchingCubesManager *engine::MarchingCubesManager::getInstance() {
    return g_instance;
}

bool engine::MarchingCubesManager::isPositionHasSolidVoxel(const glm::vec3 &position) {
    glm::ivec3 chunkPos = m_converter.worldPositionToChunkPosition(position, CHUNCK_DIMENSION_SIZE);
    if (!isChunkInbounds(chunkPos.x, chunkPos.y, chunkPos.z)) return false;
    glm::ivec2 localXZ = m_converter.worldChunkToLocalChunkPosition(chunkPos.x, chunkPos.z, m_currentOriginChunk.x, m_currentOriginChunk.y);
    glm::ivec3 localVoxel = m_converter.worldPositionToLocalVoxelPosition(position, CHUNCK_DIMENSION_SIZE);
    VoxelChunk& chunk = m_chunks[m_grid.getChunkId(localXZ.x, chunkPos.y, localXZ.y)];
    bool result = chunk.isVoxelSolid(localVoxel.x, localVoxel.y, localVoxel.z);
    return result;
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
        if (!chunk.isInUse()) continue;
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
}

void engine::MarchingCubesManager::draw(Shader& shader) {
    shader.use();
    m_textures.use();

    for (size_t z = 0; z < 6; z++){
        for (size_t x = 0; x < 6; x++){
            VoxelChunk& chunk = m_chunks[m_grid.getChunkId(x, 0, z)];
            if (chunk.getDrawCount() == 0) continue;

            glm::ivec2 worldPos = m_converter.localChunkToWorldChunkPosition(x, z, m_currentOriginChunk.x, m_currentOriginChunk.y);
            chunk.bindCommandBuffer();
            chunk.bindSSBO();
            shader.setVec3("chunkPosition", glm::vec3(worldPos.x * CHUNCK_DIMENSION_SIZE, 0, worldPos.y * CHUNCK_DIMENSION_SIZE));
            int drawCount = chunk.getDrawCount();
            //glMemoryBarrier(GL_ALL_BARRIER_BITS);
            m_marchingCubes.draw(drawCount);
        }
    }
}

bool engine::MarchingCubesManager::isChunkInbounds(int x, int y, int z) {
    int maxX = m_currentOriginChunk.x + CHUNK_MAX_X_Z_SIZE;
    int maxZ = m_currentOriginChunk.y + CHUNK_MAX_X_Z_SIZE;
    int maxY = CHUNK_MAX_Y_SIZE;
    bool result = x >= m_currentOriginChunk.x && x < maxX && z >= m_currentOriginChunk.y && z < maxZ && y >= 0 && y < maxY;
    return result;
}

bool engine::MarchingCubesManager::isPositionInbounds(const glm::vec3& position) {
    glm::ivec3 local = m_converter.worldPositionToLocalVoxelPosition(position, CHUNCK_DIMENSION_SIZE);
    int minX = m_currentOriginChunk.x * CHUNCK_DIMENSION_SIZE;
    int minZ = m_currentOriginChunk.y * CHUNCK_DIMENSION_SIZE;
    int minY = 0;
    int maxX = minX + (CHUNK_MAX_X_Z_SIZE * CHUNCK_DIMENSION_SIZE);
    int maxZ = minZ + (CHUNK_MAX_X_Z_SIZE * CHUNCK_DIMENSION_SIZE);
    int maxY = minY + (CHUNK_MAX_Y_SIZE * CHUNCK_DIMENSION_SIZE);
    return local.x >= minX && local.x < maxX && local.y >= minY && local.y < maxY && local.z >= minZ && local.z < maxZ;
}

int engine::MarchingCubesManager::signum(float x) {
    return x > 0 ? 1 : x < 0 ? -1 : 0;
}

float engine::MarchingCubesManager::mod(int position, int modulus) {
    float local = position % modulus;
    if (local < 0) {
        local += modulus;
    }
    return local;
}

float engine::MarchingCubesManager::intbound(float s, float ds) {
    if (ds < 0) {
        return intbound(-s, -ds);
    } else {
        s = mod(s, 1);
        return (1-s)/ds;
    }
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
    // Сделано на основе следующих материалов:
    // https://gamedev.stackexchange.com/questions/47362/cast-ray-to-select-block-in-voxel-game
    // https://gedge.ca/blog/2014-01-20-voxel-iteration/
    // https://github.com/thegedge/voxel_iterator/blob/main/voxel_iterator.cpp

    // если начальная позиция вне чанков или направление равно нулю, то возвращаем false
    if (!isPositionInbounds(position) || (direction.x == 0 && direction.y == 0 && direction.z == 0)) return false;

    hitPosition = m_converter.worldPositionToVoxelPosition(position, VOXEL_SIZE);
    glm::vec3 next(intbound(hitPosition.x, direction.x), intbound(hitPosition.y, direction.y), intbound(hitPosition.z, direction.z));
    //glm::vec3 next(intbound(position.x, direction.x), intbound(position.y, direction.y), intbound(position.z, direction.z));
    glm::ivec3 step(signum(direction.x), signum(direction.y), signum(direction.z));
    glm::vec3 delta(step.x / direction.x, step.y / direction.y, step.z / direction.z);
    maxDistance /= std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

    bool first = true;
    while (isPositionInbounds(hitPosition)) {
        if (!first && isPositionHasSolidVoxel(hitPosition)) return true;
        first = false;

        if(next.x < next.y) {
            if(next.x < next.z) {
                if (next.x > maxDistance) break;
                hitPosition.x += step.x;
                next.x += delta.x;
                hitFace = glm::ivec3(-step.x, 0, 0);
            } else {
                if (next.z > maxDistance) break;
                hitPosition.z += step.z;
                next.z += delta.z;
                hitFace = glm::ivec3(0, 0, -step.z);
            }
        } else if(next.y < next.z) {
            if (next.y > maxDistance) break;
            hitPosition.y += step.y;
            next.y += delta.y;
            hitFace = glm::ivec3(0, -step.y, 0);
        } else {
            if (next.z > maxDistance) break;
            hitPosition.z += step.z;
            next.z += delta.z;
            hitFace = glm::ivec3(0, 0, -step.z);
        }
    }

    return false;
}

void engine::MarchingCubesManager::setVoxel(const glm::vec3 &position, uint8_t id) {
    glm::ivec3 chunkPos = m_converter.worldPositionToChunkPosition(position, CHUNCK_DIMENSION_SIZE);
    if (!isChunkInbounds(chunkPos.x, chunkPos.y, chunkPos.z)) return;
    glm::ivec2 localXZ = m_converter.worldChunkToLocalChunkPosition(chunkPos.x, chunkPos.z, m_currentOriginChunk.x, m_currentOriginChunk.y);
    glm::ivec3 localVoxel = m_converter.worldPositionToLocalVoxelPosition(position, CHUNCK_DIMENSION_SIZE);

    size_t chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y, localXZ.y);
    VoxelChunk& chunk = m_chunks[chunkId];
    pushToUpdateQueue(chunkId);
    chunk.setVoxel(localVoxel.x, localVoxel.y, localVoxel.z, id);

    if (localVoxel.x == 0) {
        glm::ivec3 chunkX = glm::ivec3(chunkPos.x - 1, chunkPos.y, chunkPos.z);
        if (isChunkInbounds(chunkX.x, chunkX.y, chunkX.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y, localXZ.y);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(CHUNCK_DIMENSION_SIZE, localVoxel.y, localVoxel.z, id);
        }
    }

    if (localVoxel.z == 0) {
        glm::ivec3 chunkZ = glm::ivec3(chunkPos.x, chunkPos.y, chunkPos.z - 1);
        if (isChunkInbounds(chunkZ.x, chunkZ.y, chunkZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y, localXZ.y - 1);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(localVoxel.x, localVoxel.y, CHUNCK_DIMENSION_SIZE, id);
        }
    }

    if (localVoxel.y == 0) {
        glm::ivec3 chunkY = glm::ivec3(chunkPos.x, chunkPos.y - 1, chunkPos.z);
        if (isChunkInbounds(chunkY.x, chunkY.y, chunkY.z)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y - 1, localXZ.y);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(localVoxel.x, CHUNCK_DIMENSION_SIZE, localVoxel.z, id);
        }
    }

    if (localVoxel.x == 0 && localVoxel.z == 0) {
        glm::ivec3 chunkXZ = glm::ivec3(chunkPos.x - 1, chunkPos.y, chunkPos.z - 1);
        if (isChunkInbounds(chunkXZ.x, chunkXZ.y, chunkXZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y, localXZ.y - 1);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(CHUNCK_DIMENSION_SIZE, localVoxel.y, CHUNCK_DIMENSION_SIZE, id);
        }
    }

    if (localVoxel.x == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkXY = glm::ivec3(chunkPos.x - 1, chunkPos.y - 1, chunkPos.z);
        if (isChunkInbounds(chunkXY.x, chunkXY.y, chunkXY.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y - 1, localXZ.y);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(CHUNCK_DIMENSION_SIZE, CHUNCK_DIMENSION_SIZE, localVoxel.z, id);
        }
    }

    if (localVoxel.z == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkYZ = glm::ivec3(chunkPos.x, chunkPos.y - 1, chunkPos.z - 1);
        if (isChunkInbounds(chunkYZ.x, chunkYZ.y, chunkYZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x, chunkPos.y - 1, localXZ.y - 1);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(localVoxel.x, CHUNCK_DIMENSION_SIZE, CHUNCK_DIMENSION_SIZE, id);
        }
    }

    if (localVoxel.x == 0 && localVoxel.z == 0 && localVoxel.y == 0) {
        glm::ivec3 chunkXYZ = glm::ivec3(chunkPos.x - 1, chunkPos.y - 1, chunkPos.z - 1);
        if (isChunkInbounds(chunkXYZ.x, chunkXYZ.y, chunkXYZ.z)) {
            chunkId = m_grid.getChunkId(localXZ.x - 1, chunkPos.y - 1, localXZ.y - 1);
            pushToUpdateQueue(chunkId);
            m_chunks[chunkId].setVoxel(CHUNCK_DIMENSION_SIZE, CHUNCK_DIMENSION_SIZE, CHUNCK_DIMENSION_SIZE, id);
        }
    }
}

void engine::MarchingCubesManager::resizeChunkGrid(unsigned int size) {
    if (size % 2 == 1) size -= 1;
    if (size > m_grid.CHUNK_MAX_X_Z_SIZE) size = m_grid.CHUNK_MAX_X_Z_SIZE;
    unsigned int usedChunkDistance = m_grid.getUsedChunkDistance();
    if (usedChunkDistance == size) return;
    int diffirence = size - usedChunkDistance;

    if (size < usedChunkDistance) {
        std::vector<int> chunksToDelete;
        m_grid.resizeToSmaller(diffirence, chunksToDelete); 

        for (int id : chunksToDelete) {
            m_freeChunkIndices.push(id);
            m_chunks[id].setInUseFlag(false);
        }
    }
    else {
        m_toGenerateQueue.clear();
        m_grid.resizeToBigger(diffirence, m_toGenerateQueue);

        for (glm::ivec2& pos : m_toGenerateQueue) {
            for (int y = 0; y < m_grid.CHUNK_MAX_Y_SIZE; y++) {
                size_t id = m_freeChunkIndices.top();
                m_freeChunkIndices.pop();

                VoxelChunk& chunk = m_chunks[id];
                chunk.setInUseFlag(true);
                m_grid.setChunk(pos.x, y, pos.y, id);

                chunk.clear();
                m_toUpdateQueue.push(id);
                chunk.setInUpdateQueueFlag(true);

                if (y == 0) {
                    for (size_t x = 0; x < 32; x++) {
                        for (size_t z = 0; z < 32; z++){
                            m_chunks[id].setVoxel(x,1,z, 0);
                        }
                    }
                }

            }
        }
        m_toGenerateQueue.clear();
    }
}
