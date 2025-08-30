#ifndef __VOXEL__CHUNK_GRID_VISIBILITY_H__
#define __VOXEL__CHUNK_GRID_VISIBILITY_H__

#include "../stdafx.h"
#include "ChunkGridBounds.h"
#include "ChunkGrid.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"
#include "../Collisions/AABB.h"
#include "../Collisions/Frustum.h"

namespace engine {
    class ChunkGridVisibility {
    public:
        ChunkGridVisibility();

        enum class VisabilityType : uint8_t { CAMERA = 0b00000001, SUNLIGHT = 0b00000010 };

        void clearResults();
        void checkVisibility(
            const glm::vec3& cameraPosition, const glm::vec3& cameraDirection, 
            const Frustum& frustum, VisabilityType type, ChunkGrid& grid, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter
        );
        bool isVisible(int x, int y, int z, VisabilityType type) { return m_gridVisible[x][y][z] & (uint8_t)type; }
        bool isVisible(const glm::ivec3& chunkPosition, VisabilityType type) { return m_gridVisible[chunkPosition.x][chunkPosition.y][chunkPosition.z] & (uint8_t)type; }

    private:
        static constexpr uint8_t INVISIBLE_STATE = 0;
        uint8_t m_gridVisible[ChunkGridBounds::CHUNK_MAX_X_Z_SIZE][ChunkGridBounds::CHUNK_MAX_Y_SIZE][ChunkGridBounds::CHUNK_MAX_X_Z_SIZE];
        std::stack<std::pair<glm::ivec3, ChunkVisibilityState::Side>> m_stack;

        void setVisibilityStateVisible(int x, int y, int z, VisabilityType type) { m_gridVisible[x][y][z] |= (uint8_t)type; }
        void setVisibilityStateVisible(const glm::ivec3& chunkPosition, VisabilityType type) { m_gridVisible[chunkPosition.x][chunkPosition.y][chunkPosition.z] |= (uint8_t)type; }

        /// @brief Исправление видимости краёв чанков, использующих воксели соседних чанков.
        void borderVisibilityFix(
            const Frustum& frustum, VisabilityType type, ChunkGrid &grid, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter
        );

        /// @brief Исправление видимости краёв чанков на одной высоте с текущим чанком.
        /// @param chunk - Объект текущего чанка.
        /// @param x, y, z - Позиция текущего чанка.
        /// @param isBackAvailable, isLeftAvailable - Возвращаемые значения видимости боковых чанков. 
        void borderVisibilityFixGroundLevel(
            const Frustum& frustum, VisabilityType type, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter,
            VoxelChunk& chunk, int x, int y, int z, bool& isBackAvailable, bool& isLeftAvailable
        );

        /// @brief Исправление видимости краёв чанков на один чанк ниже текущего.
        /// @param chunk - Объект текущего чанка.
        /// @param x, y, z - Позиция текущего чанка.
        /// @param isBackAvailable, isLeftAvailable - Значения видимости боковых чанков.
        void borderVisibilityFixUndergroundLevel(
            const Frustum& frustum, VisabilityType type, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter,
            VoxelChunk& chunk, int x, int y, int z, bool isBackAvailable, bool isLeftAvailable
        );

        /// @brief Проверка наличия чанка в пирамиде отсечения.
        /// @param chunkPosition Позиция проверяемого чанка.
        /// @param frustum Объект пирамиды отсечения.
        /// @param gridBounds Объект класса определения границ текущего воксельного поля.
        /// @param converter Конвертер координат.
        /// @return True если чанк находится в пирамиде отсечения или соприкасается с ней. False в ином случае.
        bool isChunkInFrustum(const glm::ivec3& chunkPosition, const Frustum& frustum, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter);
        
        /// @brief Функция поиска ближайшего к лучу чанка на границах воксельного поля. Данная функция используется для дебага.
        /// @param cameraPosition Позиция камеры.
        /// @param cameraDirection Направление камеры.
        /// @param frustum Объект пирамиды отсечения.
        /// @param gridBounds Объект класса определения границ текущего воксельного поля.
        /// @param converter Конвертер координат.
        /// @return Позиция чанка в локальной системе координат чанков. Если луч не пересекается с сеткой чанков
        ///         будет возвращено значение (0,0,0).
        glm::ivec3 raycastBorderChunk(
            const glm::vec3 &cameraPosition, const glm::vec3& cameraDirection, const Frustum& frustum, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter
        );
        
        /// @brief Функция добаления ближайших к камере чанков в очередь проверки видимости.
        /// @param cameraPosition Позиция камеры.
        /// @param localCameraChunk Локальная для сетки позиция чанка, в котором находится камера.
        /// @param frustum Объект пирамиды отсечения.
        /// @param grid Объект сетки чанков.
        /// @param gridBounds Объект класса определения границ текущего воксельного поля.
        /// @param converter Конвертер координат.
        void queueCloseToCameraChunks(
            const glm::vec3& cameraPosition, const glm::vec3& localCameraChunk, 
            const Frustum& frustum, ChunkGrid& grid, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter
        );

        /// @brief Функция добавления всех чанков на границах воксельного поля в очередь проверки видимости.
        ///        В очередь добавляюся только чанки с ближайших к камере границ.
        ///        Данная функция используется если камера находится за границами воксельного поля.
        /// @param cameraPosition Позиция камеры.
        /// @param frustum Объект пирамиды отсечения.
        /// @param gridBounds Объект класса определения границ текущего воксельного поля.
        /// @param converter Конвертер координат.
        void queueBorderChunks(const glm::vec3& cameraPosition, const Frustum& frustum, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter);

        /// @brief Функция поиска соседних чанков и определения направлений к ним.
        /// @param position Позиция чанка, для которого происходит поиск соседей.
        /// @param gridBounds Объект класса определения границ текущего воксельного поля.
        /// @param allowGo Флаги, разрешающие или запрещающие поиск в определённых направлениях.
        /// @return Массив до 6 соседних чанков в виде пар позиций и направлений к ним. Если направление 
        ///         элемента массива указано как NONE, то этот и все последующие элементы массива не содержат данных.
        std::array<std::pair<glm::ivec3, ChunkVisibilityState::Side>, 7> findNeighbours(
            const glm::ivec3& position, const ChunkGridBounds& gridBounds, 
            bool allowGoUp, bool allowGoDown, bool allowGoFront, bool allowGoBack, bool allowGoRight, bool allowGoLeft
        );

        /// @brief Хелпер получения противоположного направления.
        /// @param side Направление.
        /// @return Противоположное направление (LEFT <-> RIGHT, TOP <-> BOTTOM, FRONT <-> BACK).
        ChunkVisibilityState::Side getMirroredVisibilitySide(ChunkVisibilityState::Side side);

    };
}

#endif