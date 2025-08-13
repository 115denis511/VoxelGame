#ifndef __VOXEL__CHUNK_GRID_BOUNDS_H__
#define __VOXEL__CHUNK_GRID_BOUNDS_H__

#include "../stdafx.h"

namespace engine {
    struct ChunkGridBounds {
        static constexpr int CHUNCK_DIMENSION_SIZE = 32 - 1;
        static constexpr int VOXEL_SIZE = 1;
        static constexpr int CHUNK_MAX_X_Z_SIZE = 24;
        static constexpr int CHUNK_MAX_Y_SIZE = 8;
        static constexpr unsigned int MAX_RENDER_CHUNK_RADIUS = CHUNK_MAX_X_Z_SIZE / 2;
        static constexpr int CHUNK_COUNT = CHUNK_MAX_X_Z_SIZE * CHUNK_MAX_X_Z_SIZE * CHUNK_MAX_Y_SIZE;
        /* Позиция чанка[0, y, 0] из сетки чанков в мировых координатах.
           x - позиция чанка по оси X, y - позиция чанка по оси Z. */
        glm::ivec2 currentOriginChunk{ glm::ivec2( -(CHUNK_MAX_X_Z_SIZE / 2), -(CHUNK_MAX_X_Z_SIZE / 2)) };
        glm::ivec2 currentCenterChunk{ glm::ivec2( 0, 0) };
        unsigned int usedChunkGridWidth{ 0 };

        /// @brief Проверка нахождения мировой позиции чанка в внутри текущих границ сетки чанков.
        /// @param x,y,z Позиция чанка в мировых координатах.
        /// @return True если чанк внутри границ сетки. False если нет.
        bool isWorldChunkInbounds(int x, int y, int z) const;

        /// @brief Проверка нахождения мировой позиции в внутри текущих границ сетки чанков.
        /// @param position Позиция в мировых координатах.
        /// @return True если позиция внутри границ сетки. False если нет.
        bool isWorldPositionInbounds(const glm::vec3& position) const;

        inline bool isChunkYInbounds(unsigned int y) const { return y >= 0 && y < CHUNK_MAX_Y_SIZE; }
        inline bool isChunkXZInbounds(unsigned int x, unsigned int z) const { return x >= 0 && x < usedChunkGridWidth && z >= 0 && z < usedChunkGridWidth; }
    };
}

#endif
