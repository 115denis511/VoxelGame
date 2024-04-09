#include "ChunkGrid.h"

engine::ChunkGrid::ChunkGrid() {
    for (int i = 0; i < CHUNK_MAX_X_Z_SIZE; i++) {
        for (int j = 0; j < CHUNK_MAX_X_Z_SIZE; j++) {
            m_grid[i][j] = GridYSlice();
        }
    }
}

int engine::ChunkGrid::getChunkId(int x, int y, int z) {
    assert(x >= 0 && x < CHUNK_MAX_X_Z_SIZE && z >= 0 && z < CHUNK_MAX_X_Z_SIZE && y >= 0 && y < CHUNK_MAX_Y_SIZE);
    return m_grid[x][z].chunk[y];
}

void engine::ChunkGrid::setChunk(int x, int y, int z, int id) {
    m_grid[x][z].chunk[y] = id;
}

void engine::ChunkGrid::resizeToBigger(unsigned int distance, std::vector<glm::ivec2> &chunksToCreate) {
    assert(distance > m_usedChunkDistance);

    // Если старая сетка была пуста, то вся расширенная сетка отправляется в очередь для генерации
    if (m_usedChunkDistance == 0) {
        for (size_t i = 0; i < distance; i++) {
            for (size_t j = 0; j < distance; j++) {
                chunksToCreate.push_back(glm::ivec2(i, j));
            }
        }
        m_usedChunkDistance = distance;
        return;
    }

    // Перемещение имеющихся чанков в центр расширенной сетки
    unsigned int halfDiffirence = (distance - m_usedChunkDistance) / 2;
    for (size_t x = m_usedChunkDistance - 1; x >= 0; x--) {
        for (size_t z = m_usedChunkDistance - 1; z >= 0; z--) {
            m_grid[x + halfDiffirence][z + halfDiffirence] = m_grid[x][z];
            m_grid[x][z] = GridYSlice();
        }
    }
    // Поиск пустых чанков и добавление их в очередь для генерации
    for (size_t x = 0; x < distance; x++) {
        for (size_t z = 0; z < distance; z++) {
            if (m_grid[x][z].chunk[0] == -1) {
                chunksToCreate.push_back(glm::ivec2(x, z));
            }
        }
    }
    m_usedChunkDistance = distance;
}

void engine::ChunkGrid::resizeToSmaller(unsigned int distance, std::vector<int> &chunksToDelete) {
    assert(distance < m_usedChunkDistance);

    // Если размер именённой сетки будет равен нулю, то отправляем все чанки на удаление
    if (distance == 0) {
        for (size_t x = 0; x < m_usedChunkDistance; x++) {
            for (size_t z = 0; z < m_usedChunkDistance; z++) {
                for (size_t y = 0; y < CHUNK_MAX_Y_SIZE; y++) {
                    assert(m_grid[x][z].chunk[y] != -1);
                    chunksToDelete.push_back(m_grid[x][z].chunk[y]);
                }
            }
        }
        m_usedChunkDistance = distance;
        return;
    }

    // Обрезаем края, оказавшиеся вне сетки
    // Обрезание сделано под впечатлением от алгоритма Брезенхэма для окужностей.
    // Суть в том, что итерируя по чанкам вокруг фигуры, зная её размеры, можно вычислить
    // позиции чанков в противоположных краях фигуры. Таким образом можно обойти сразу 4 
    // края чанка за одну итерацию цикла, главное ограничить итерацию непересекающимися фигурами.
    unsigned int halfDiffirence = (m_usedChunkDistance - distance) / 2;
    for (size_t i = 0; i < m_usedChunkDistance - halfDiffirence; i++) {
        for (size_t j = 0; j < halfDiffirence; j++) {
            for (size_t y = 0; y < CHUNK_MAX_Y_SIZE; y++) {
                chunksToDelete.push_back(m_grid[i][j].chunk[y]);
                m_grid[i][j].chunk[y] = -1;

                unsigned int iMirror = m_usedChunkDistance - 1 - i;
                unsigned int jMirror = m_usedChunkDistance - 1 - j;
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
    for (size_t x = 0; x < distance; x++) {
        for (size_t z = 0; z < distance; z++) {
            unsigned int oldX = x + halfDiffirence, 
                         oldZ = z + halfDiffirence;
            m_grid[x][z] = m_grid[oldX][oldZ];
            m_grid[oldX][oldZ] = GridYSlice();
        }
    }
    m_usedChunkDistance = distance;
}
