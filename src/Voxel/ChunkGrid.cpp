#include "ChunkGrid.h"

engine::ChunkGrid::ChunkGrid(VoxelChunk (&chunks)[ChunkGridBounds::CHUNK_COUNT]) 
    : m_chunks(chunks) 
{
    for (int i = 0; i < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE; i++) {
        for (int j = 0; j < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE; j++) {
            m_grid[i][j] = GridYSlice();
        }
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

engine::VoxelChunk &engine::ChunkGrid::getChunk(int x, int y, int z) {
    return m_chunks[getChunkId(x, y, z)];
}

void engine::ChunkGrid::setChunk(int x, int y, int z, int id) {
    m_grid[x][z].chunk[y] = id;
}

void engine::ChunkGrid::resizeToBigger(int distance, ChunkGridBounds& gridBounds, std::vector<glm::ivec2> &chunksToCreate) {
    assert(distance > (int)gridBounds.usedChunkGridWidth);
    unsigned int uDistance = (unsigned int)distance;

    // Если старая сетка была пуста, то вся расширенная сетка отправляется в очередь для генерации
    if (gridBounds.usedChunkGridWidth == 0) {
        for (size_t i = 0; i < uDistance; i++) {
            for (size_t j = 0; j < uDistance; j++) {
                chunksToCreate.push_back(glm::ivec2(i, j));
            }
        }
        gridBounds.usedChunkGridWidth = uDistance;
        return;
    }

    // Перемещение имеющихся чанков в центр расширенной сетки
    unsigned int halfDiffirence = ((unsigned int)distance - gridBounds.usedChunkGridWidth) / 2;
    // счётчики должны быть INT потому что могут быть отрицательные значения!
    for (int x = gridBounds.usedChunkGridWidth - 1; x >= 0; x--) {
        for (int z = gridBounds.usedChunkGridWidth - 1; z >= 0; z--) {
            m_grid[x + halfDiffirence][z + halfDiffirence] = m_grid[x][z];
            m_grid[x][z] = GridYSlice();
        }
    }
    // Поиск пустых чанков и добавление их в очередь для генерации
    for (size_t x = 0; x < (unsigned int)distance; x++) {
        for (size_t z = 0; z < (unsigned int)distance; z++) {
            if (m_grid[x][z].chunk[0] == -1) {
                chunksToCreate.push_back(glm::ivec2(x, z));
            }
        }
    }
    gridBounds.usedChunkGridWidth = (unsigned int)distance;
}

void engine::ChunkGrid::resizeToSmaller(int distance, ChunkGridBounds& gridBounds, std::vector<int> &chunksToDelete) {
    assert(distance < (int)gridBounds.usedChunkGridWidth);

    // Если размер именённой сетки будет равен нулю, то отправляем все чанки на удаление
    if (distance <= 0) {
        for (size_t x = 0; x < gridBounds.usedChunkGridWidth; x++) {
            for (size_t z = 0; z < gridBounds.usedChunkGridWidth; z++) {
                for (size_t y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
                    assert(m_grid[x][z].chunk[y] != -1);
                    chunksToDelete.push_back(m_grid[x][z].chunk[y]);
                }
            }
        }
        gridBounds.usedChunkGridWidth = (unsigned int)distance;
        return;
    }

    // Обрезаем края, оказавшиеся вне сетки
    // Обрезание сделано под впечатлением от алгоритма Брезенхэма для окужностей.
    // Суть в том, что итерируя по чанкам вокруг фигуры, зная её размеры, можно вычислить
    // позиции чанков в противоположных краях фигуры. Таким образом можно обойти сразу 4 
    // края чанка за одну итерацию цикла, главное ограничить итерацию непересекающимися фигурами.
    unsigned int halfDiffirence = (gridBounds.usedChunkGridWidth - distance) / 2;
    for (size_t i = 0; i < gridBounds.usedChunkGridWidth - halfDiffirence; i++) {
        for (size_t j = 0; j < halfDiffirence; j++) {
            for (size_t y = 0; y < ChunkGridBounds::CHUNK_MAX_Y_SIZE; y++) {
                chunksToDelete.push_back(m_grid[i][j].chunk[y]);
                m_grid[i][j].chunk[y] = -1;

                unsigned int iMirror = gridBounds.usedChunkGridWidth - 1 - i;
                unsigned int jMirror = gridBounds.usedChunkGridWidth - 1 - j;
                chunksToDelete.push_back(m_grid[iMirror][jMirror].chunk[y]);
                m_grid[iMirror][jMirror].chunk[y] = -1;

                chunksToDelete.push_back(m_grid[j][iMirror].chunk[y]);
                m_grid[j][iMirror].chunk[y] = -1;

                chunksToDelete.push_back(m_grid[jMirror][i].chunk[y]);
                m_grid[jMirror][i].chunk[y] = -1;
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
    gridBounds.usedChunkGridWidth = distance;
}

bool engine::ChunkGrid::isPositionHasSolidVoxel(const glm::vec3 &position, const ChunkGridBounds& gridBounds) {
    glm::ivec3 chunkPos = VoxelPositionConverter::worldPositionToChunkPosition(position, gridBounds.CHUNCK_DIMENSION_SIZE);
    if (!gridBounds.isChunkInbounds(chunkPos.x, chunkPos.y, chunkPos.z)) return false;
    glm::ivec2 localXZ = VoxelPositionConverter::worldChunkToLocalChunkPosition(chunkPos.x, chunkPos.z, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
    glm::ivec3 localVoxel = VoxelPositionConverter::worldPositionToLocalVoxelPosition(position, gridBounds.CHUNCK_DIMENSION_SIZE);
    VoxelChunk& chunk = getChunk(localXZ.x, chunkPos.y, localXZ.y);
    bool result = chunk.isVoxelSolid(localVoxel.x, localVoxel.y, localVoxel.z);
    return result;
}
