#include "ChunkGrid.h"

engine::ChunkGrid::ChunkGrid(
    ChunkGridBounds& gridBounds, 
    ShaderStorageBuffer<glm::ivec4>& chunkPositionsSSBO,
    VoxelPositionConverter& converter
) 
    : m_gridBounds(gridBounds), m_chunkPositionsSSBO(chunkPositionsSSBO), m_converter(converter)
{
    for (int i = 0; i < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE; i++) {
        for (int j = 0; j < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE; j++) {
            m_grid[i][j] = GridYSlice();
        }
    }

    for (size_t i = 0; i < std::size(m_chunks); i++) {
        m_freeChunkIndices.push(i);
    }
}

void engine::ChunkGrid::initChunksSSBO() {
    for (auto& chunk : m_chunks) {
        chunk.initSSBO();
    }
}

void engine::ChunkGrid::initChunkLocationsInSSBO() {
    for (int i = 0; i < ChunkGridBounds::CHUNK_COUNT; i++) {
        m_chunks[i].setIdInSSBO(i);
    }
}

void engine::ChunkGrid::freeChunksSSBO() {
    for (auto& chunk : m_chunks) {
        chunk.freeSSBO();
    }
}

int engine::ChunkGrid::getChunkId(int x, int y, int z) {
    assert(
        x >= 0 && x < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE && 
        z >= 0 && z < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE && 
        y >= 0 && y < ChunkGridBounds::CHUNK_MAX_Y_SIZE
    );

    return m_grid[x][z].chunk[y];
}

engine::VoxelChunk &engine::ChunkGrid::getChunk(glm::ivec3 &localPosition) {
    return m_chunks[getChunkId(localPosition.x, localPosition.y, localPosition.z)];
}

engine::VoxelChunk &engine::ChunkGrid::getChunk(int x, int y, int z) {
    return m_chunks[getChunkId(x, y, z)];
}

engine::VoxelChunk &engine::ChunkGrid::getChunk(int id) {
    return m_chunks[id];
}

engine::VoxelChunk &engine::ChunkGrid::allocateChunk(int x, int y, int z) {
    size_t id = m_freeChunkIndices.top();
    m_freeChunkIndices.pop();

    VoxelChunk& chunk = m_chunks[id];
    chunk.setInUseFlag(true);
    m_grid[x][z].chunk[y] = id;

    m_minUpdated = std::min(m_minUpdated, id);
    m_maxUpdated = std::max(m_maxUpdated, id);

    glm::ivec2 worldPosition = m_converter.localChunkToWorldChunkPosition(
        x, 
        z, 
        m_gridBounds.currentOriginChunk.x, 
        m_gridBounds.currentOriginChunk.y
    );
    m_chunkPositions[id] = glm::ivec4(
        worldPosition.x * m_gridBounds.CHUNCK_DIMENSION_SIZE, 
        y * m_gridBounds.CHUNCK_DIMENSION_SIZE, 
        worldPosition.y * m_gridBounds.CHUNCK_DIMENSION_SIZE, 
        0
    );

    return chunk;
}

void engine::ChunkGrid::freeChunk(int x, int y, int z) {
    int id = getChunkId(x, y, z);
    freeChunk(id);

    m_grid[x][z].chunk[y] = -1;
}

void engine::ChunkGrid::freeChunk(int id) {
    assert(id >= 0 && id < ChunkGridBounds::CHUNK_COUNT && "Try to free incorrect id");
    m_freeChunkIndices.push(id);
    m_chunks[id].setInUseFlag(false);
}

bool engine::ChunkGrid::isHaveChunk(int x, int y, int z) {
    return m_grid[x][z].chunk[y] >= 0;
}

void engine::ChunkGrid::syncGpuChunkPositions() {
    if (m_minUpdated > m_maxUpdated) return;

    assert(m_chunkPositionsSSBO.isInited() && "Trying to sync positions with not inited SSBO!");
    GLsizei count = static_cast<GLsizei>(m_maxUpdated - m_minUpdated) + 1;
    GLsizei startByte = m_minUpdated * sizeof(glm::ivec4);
    m_chunkPositionsSSBO.pushData(&m_chunkPositions[m_minUpdated], count, startByte);

    m_minUpdated = UINT_MAX, m_maxUpdated = 0;
}

void engine::ChunkGrid::resizeToBigger(int distance, std::vector<glm::ivec2> &chunksToCreate) {
    assert(distance > (int)m_gridBounds.usedChunkGridWidth);
    unsigned int uDistance = (unsigned int)distance;

    // Если старая сетка была пуста, то вся расширенная сетка отправляется в очередь для генерации
    if (m_gridBounds.usedChunkGridWidth == 0) {
        for (size_t i = 0; i < uDistance; i++) {
            for (size_t j = 0; j < uDistance; j++) {
                glm::ivec2 worldPosition = m_converter.localChunkToWorldChunkPosition(i, j, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
                chunksToCreate.push_back(worldPosition);
                //chunksToCreate.push_back(glm::ivec2(i, j));
            }
        }
        m_gridBounds.usedChunkGridWidth = uDistance;
        return;
    }

    // Перемещение имеющихся чанков в центр расширенной сетки
    unsigned int halfDiffirence = ((unsigned int)distance - m_gridBounds.usedChunkGridWidth) / 2;
    // счётчики должны быть INT потому что могут быть отрицательные значения!
    for (int x = m_gridBounds.usedChunkGridWidth - 1; x >= 0; x--) {
        for (int z = m_gridBounds.usedChunkGridWidth - 1; z >= 0; z--) {
            m_grid[x + halfDiffirence][z + halfDiffirence] = m_grid[x][z];
            m_grid[x][z] = GridYSlice();
        }
    }
    // Поиск пустых чанков и добавление их в очередь для генерации
    for (size_t x = 0; x < (unsigned int)distance; x++) {
        for (size_t z = 0; z < (unsigned int)distance; z++) {
            if (m_grid[x][z].chunk[0] == -1) {
                glm::ivec2 worldPosition = m_converter.localChunkToWorldChunkPosition(x, z, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
                chunksToCreate.push_back(worldPosition);
                //chunksToCreate.push_back(glm::ivec2(x, z));
            }
        }
    }
    m_gridBounds.usedChunkGridWidth = (unsigned int)distance;
}

void engine::ChunkGrid::resizeToSmaller(int distance, std::vector<int> &chunksToDelete) {
    assert(distance < (int)m_gridBounds.usedChunkGridWidth);

    // Если размер именённой сетки будет равен нулю, то отправляем все чанки на удаление
    if (distance <= 0) {
        for (size_t x = 0; x < m_gridBounds.usedChunkGridWidth; x++) {
            for (size_t z = 0; z < m_gridBounds.usedChunkGridWidth; z++) {
                for (size_t y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
                    if (m_grid[x][z].chunk[y] != -1) {
                        chunksToDelete.push_back(m_grid[x][z].chunk[y]);
                        m_grid[x][z].chunk[y] = -1;
                    }
                }
            }
        }
        m_gridBounds.usedChunkGridWidth = (unsigned int)distance;
        return;
    }

    // Обрезаем края, оказавшиеся вне сетки
    // Обрезание сделано под впечатлением от алгоритма Брезенхэма для окужностей.
    // Суть в том, что итерируя по чанкам вокруг фигуры, зная её размеры, можно вычислить
    // позиции чанков в противоположных краях фигуры. Таким образом можно обойти сразу 4 
    // края чанка за одну итерацию цикла, главное ограничить итерацию непересекающимися фигурами.
    unsigned int halfDiffirence = (m_gridBounds.usedChunkGridWidth - distance) / 2;
    for (size_t i = 0; i < m_gridBounds.usedChunkGridWidth - halfDiffirence; i++) {
        for (size_t j = 0; j < halfDiffirence; j++) {
            for (size_t y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
                if (m_grid[i][j].chunk[y] != -1) {
                    chunksToDelete.push_back(m_grid[i][j].chunk[y]);
                    m_grid[i][j].chunk[y] = -1;
                }

                unsigned int iMirror = m_gridBounds.usedChunkGridWidth - 1 - i;
                unsigned int jMirror = m_gridBounds.usedChunkGridWidth - 1 - j;

                if (m_grid[iMirror][jMirror].chunk[y] != -1) {
                    chunksToDelete.push_back(m_grid[iMirror][jMirror].chunk[y]);
                    m_grid[iMirror][jMirror].chunk[y] = -1;
                }
                if (m_grid[j][iMirror].chunk[y] != -1) {
                    chunksToDelete.push_back(m_grid[j][iMirror].chunk[y]);
                    m_grid[j][iMirror].chunk[y] = -1;
                }
                if (m_grid[jMirror][i].chunk[y] != -1) {
                    chunksToDelete.push_back(m_grid[jMirror][i].chunk[y]);
                    m_grid[jMirror][i].chunk[y] = -1;
                }
            }
        }
    }
    // Сдвигаем оставшиеся чанки к левому верхнему краю
    for (size_t x = 0; x < (unsigned int)distance; x++) {
        for (size_t z = 0; z < (unsigned int)distance; z++) {
            unsigned int oldX = x + halfDiffirence, 
                         oldZ = z + halfDiffirence;
            m_grid[x][z] = m_grid[oldX][oldZ];
            m_grid[oldX][oldZ] = GridYSlice();
        }
    }
    m_gridBounds.usedChunkGridWidth = distance;
}

bool engine::ChunkGrid::isPositionHasSolidVoxel(const glm::vec3 &position) {
    glm::ivec3 chunkPos = VoxelPositionConverter::worldPositionToChunkPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);
    if (!m_gridBounds.isWorldChunkInbounds(chunkPos.x, chunkPos.y, chunkPos.z)) return false;

    glm::ivec2 localXZ = VoxelPositionConverter::worldChunkToLocalChunkPosition(chunkPos.x, chunkPos.z, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
    glm::ivec3 localVoxel = VoxelPositionConverter::worldPositionToLocalVoxelPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);
    if (!isHaveChunk(localXZ.x, chunkPos.y, localXZ.y)) return false;
    VoxelChunk& chunk = getChunk(localXZ.x, chunkPos.y, localXZ.y);

    bool result = chunk.isVoxelSolid(localVoxel.x, localVoxel.y, localVoxel.z);
    return result;
}
