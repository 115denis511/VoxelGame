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
            const Frustum& frustum, VisabilityType type, ChunkGrid& grid, const ChunkGridBounds &gridBounds
        );
        bool isVisible(int x, int y, int z, VisabilityType type) { return m_gridVisible[x][y][z] & (uint8_t)type; }
        bool isVisible(const glm::ivec3& chunkPosition, VisabilityType type) { return m_gridVisible[chunkPosition.x][chunkPosition.y][chunkPosition.z] & (uint8_t)type; }

    private:
        static constexpr uint8_t INVISIBLE_STATE = 0;
        uint8_t m_gridVisible[ChunkGridBounds::CHUNK_MAX_X_Z_SIZE][ChunkGridBounds::CHUNK_MAX_Y_SIZE][ChunkGridBounds::CHUNK_MAX_X_Z_SIZE];
        std::stack<std::pair<glm::ivec3, ChunkVisibilityState::Side>> m_stack;

        void setVisibilityStateVisible(int x, int y, int z, VisabilityType type) { m_gridVisible[x][y][z] |= (uint8_t)type; }
        void setVisibilityStateVisible(const glm::ivec3& chunkPosition, VisabilityType type) { m_gridVisible[chunkPosition.x][chunkPosition.y][chunkPosition.z] |= (uint8_t)type; }

        /// @brief Проверка наличия чанка в пирамиде отсечения.
        /// @param chunkPosition Позиция проверяемого чанка.
        /// @param frustum Объект пирамиды отсечения.
        /// @param gridBounds Объект класса определения границ текущего воксельного поля.
        /// @return True если чанк находится в пирамиде отсечения или соприкасается с ней. False в ином случае.
        bool isChunkInFrustum(const glm::ivec3& chunkPosition, const Frustum& frustum, const ChunkGridBounds &gridBounds);
        
        glm::ivec3 findClosestVisibleChunkForCameraOutsideBounds(
            const glm::vec3 &cameraPosition, const glm::vec3& cameraDirection, const Frustum& frustum, const ChunkGridBounds &gridBounds
        );

        /// @brief Функция поиска соседних чанков и определения направлений к ним.
        /// @param position Позиция чанка, для которого происходит поиск соседей.
        /// @param gridBounds Объект класса определения границ текущего воксельного поля.
        /// @param allowGo Флаги, разрещающие или запрещающие поиск в определённых направлениях.
        /// @return Массив до 6 соседних чанков в виде пар позиций и направлений к ним. Если направление 
        ///         элемента массива указано как NONE, то этот и все последующие элементы не содержат данных.
        std::array<std::pair<glm::ivec3, ChunkVisibilityState::Side>, 7> findNeighbours(
            const glm::ivec3& position, const ChunkGridBounds& gridBounds, 
            bool allowGoUp, bool allowGoDown, bool allowGoFront, bool allowGoBack, bool allowGoRight, bool allowGoLeft
        );

        /// @brief Хелпер получения противоположного значения стороны.
        /// @param side Сторона чанка.
        /// @return Противоположная сторона чанка (LEFT <-> RIGHT, TOP <-> BOTTOM, FRONT <-> BACK).
        ChunkVisibilityState::Side getMirroredVisibilitySide(ChunkVisibilityState::Side side);

    };
}

#endif