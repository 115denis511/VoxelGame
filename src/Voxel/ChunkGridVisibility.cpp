#include "ChunkGridVisibility.h"

engine::ChunkGridVisibility::ChunkGridVisibility() {
    clearResults();
}

void engine::ChunkGridVisibility::clearResults() {
    for (int i = 0; i < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE; i++) {
        for (int j = 0; j < ChunkGridBounds::CHUNK_MAX_Y_SIZE; j++) {
            for (int k = 0; k < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE; k++) {
                m_gridVisible[i][j][k] = INVISIBLE_STATE;
            }
        }
    }
}

void engine::ChunkGridVisibility::checkVisibility(
    const glm::vec3 &cameraPosition, const glm::vec3 &cameraDirection, 
    const Frustum& frustum, VisabilityType type, ChunkGrid &grid, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter
) {
    // Сделано на основе следующих материалов:
    // https://tomcc.github.io/2014/08/31/visibility-1.html
    // https://tomcc.github.io/2014/08/31/visibility-2.html
    glm::ivec3 cameraChunk = converter.worldPositionToChunkPosition(cameraPosition, gridBounds.CHUNCK_DIMENSION_SIZE);
    glm::ivec3 cameraChunkLocal = converter.worldChunkToLocalChunkPosition(cameraChunk, gridBounds.currentOriginChunk);

    if (gridBounds.usedChunkGridWidth == 0) return;

    clearVisitedDirections();

    if (gridBounds.isWorldChunkInbounds(cameraChunk.x, cameraChunk.y, cameraChunk.z)) {
        m_stack.push(std::make_pair(cameraChunkLocal, ChunkVisibilityState::Side::NONE));
    } 
    else {
        queueBorderChunks(cameraPosition, frustum, gridBounds, converter);
        /*m_stack.push(std::make_pair(
            raycastBorderChunk(cameraPosition, cameraDirection, frustum, gridBounds, converter), 
            ChunkVisibilityState::Side::NONE
        ));*/
    }

    while (!m_stack.empty()) {
        glm::ivec3 currentChunk = m_stack.top().first;
        ChunkVisibilityState::Side cameFrom = getMirroredVisibilitySide(m_stack.top().second);
        m_stack.pop(); 

        setVisibilityStateVisible(currentChunk, type);

        auto neighbours = findNeighbours(
            currentChunk, gridBounds, 
            currentChunk.y >= cameraChunkLocal.y, currentChunk.y <= cameraChunkLocal.y,
            currentChunk.z >= cameraChunkLocal.z, currentChunk.z <= cameraChunkLocal.z,
            currentChunk.x >= cameraChunkLocal.x, currentChunk.x <= cameraChunkLocal.x
        );
        for (int i = 0; neighbours[i].second != ChunkVisibilityState::Side::NONE; i++) {
            glm::ivec3 neighbourChunk = neighbours[i].first;
            ChunkVisibilityState::Side cameTo = neighbours[i].second;

            if (isDirectionVisited(neighbourChunk, cameTo)) continue;

            VoxelChunk& chunk = grid.getChunk(currentChunk.x, currentChunk.y, currentChunk.z);
            if (!chunk.isVisibleThrough(cameFrom, cameTo)) { continue; }

            if (!isChunkInFrustum(neighbourChunk, frustum, gridBounds, converter)) { continue; }

            m_stack.push(std::make_pair(neighbourChunk, cameTo));
            setVisitedDirection(neighbourChunk, cameTo, true);
        }
    }
}

void engine::ChunkGridVisibility::clearVisitedDirections() {
    for (int i = 0; i < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE; i++) {
        for (int j = 0; j < ChunkGridBounds::CHUNK_MAX_Y_SIZE; j++) {
            for (int k = 0; k < ChunkGridBounds::CHUNK_MAX_X_Z_SIZE; k++) {
                m_visitedDirection[i][j][k][0] = false;
                m_visitedDirection[i][j][k][1] = false;
                m_visitedDirection[i][j][k][2] = false;
                m_visitedDirection[i][j][k][3] = false;
                m_visitedDirection[i][j][k][4] = false;
                m_visitedDirection[i][j][k][5] = false;
                m_visitedDirection[i][j][k][6] = false;
            }
        }
    }
}

bool engine::ChunkGridVisibility::isChunkInFrustum(
    const glm::ivec3 &chunkPosition, const Frustum &frustum, const ChunkGridBounds &gridBounds, VoxelPositionConverter &converter)
{
    glm::ivec2 worldChunkPos = converter.localChunkToWorldChunkPosition(
        chunkPosition.x, chunkPosition.z, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y
    );
    unsigned int height = chunkPosition.y * gridBounds.CHUNCK_DIMENSION_SIZE;

    glm::vec3 worldPos(
        worldChunkPos.x * gridBounds.CHUNCK_DIMENSION_SIZE, 
        height, 
        worldChunkPos.y * gridBounds.CHUNCK_DIMENSION_SIZE
    );
    AABB aabb(
        glm::vec3(worldPos), 
        glm::vec3(
            worldPos.x + gridBounds.CHUNCK_DIMENSION_SIZE, 
            worldPos.y + gridBounds.CHUNCK_DIMENSION_SIZE, 
            worldPos.z + gridBounds.CHUNCK_DIMENSION_SIZE
        )
    );

    if (aabb.isInFrustum(frustum)) return true;
    else return false;
}

glm::ivec3 engine::ChunkGridVisibility::raycastBorderChunk(
    const glm::vec3 &cameraPosition, const glm::vec3& cameraDirection, const Frustum &frustum, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter
) {
    float halfWidth = (gridBounds.usedChunkGridWidth * gridBounds.CHUNCK_DIMENSION_SIZE) / 2;
    float halfHeight = (gridBounds.CHUNK_MAX_Y_SIZE * gridBounds.CHUNCK_DIMENSION_SIZE) / 2;
    glm::vec3 gridCenter = glm::vec3(
        gridBounds.currentOriginChunk.x * gridBounds.CHUNCK_DIMENSION_SIZE + halfWidth, 
        halfHeight, 
        gridBounds.currentOriginChunk.y * gridBounds.CHUNCK_DIMENSION_SIZE + halfWidth
    );
    AABB aabb(gridCenter, halfWidth, halfHeight, halfWidth);

    float distance = 0.f;
    bool isIntersects = aabb.rayIntersects(cameraPosition, cameraDirection, distance);

    if (!isIntersects) {
        return glm::ivec3(0);
    }
    
    glm::vec3 position = cameraPosition + cameraDirection * (distance + 1.f);
    glm::ivec3 iPosition = converter.worldPositionToChunkPosition(position, gridBounds.CHUNCK_DIMENSION_SIZE);
    iPosition = converter.worldChunkToLocalChunkPosition(iPosition.x, iPosition.y, iPosition.z, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);

    if (iPosition.x < 0) { iPosition.x = 0; }
    else if (iPosition.x >= (int)gridBounds.usedChunkGridWidth) { iPosition.x = (int)gridBounds.usedChunkGridWidth - 1; }
    if (iPosition.y < 0) { iPosition.y = 0; }
    else if (iPosition.y > (int)gridBounds.CHUNK_MAX_Y_SIZE) { iPosition.y = gridBounds.CHUNK_MAX_Y_SIZE - 1; }
    if (iPosition.z < 0) { iPosition.z = 0; }
    else if (iPosition.z >= (int)gridBounds.usedChunkGridWidth) { iPosition.z = (int)gridBounds.usedChunkGridWidth - 1; }

    return iPosition;
}

void engine::ChunkGridVisibility::queueBorderChunks(
    const glm::vec3 &cameraPosition, const Frustum &frustum, const ChunkGridBounds &gridBounds, VoxelPositionConverter& converter
) {
    unsigned int borderX = 0;
    if (cameraPosition.x > gridBounds.currentCenterChunk.x * gridBounds.CHUNCK_DIMENSION_SIZE) borderX = gridBounds.usedChunkGridWidth - 1;

    for (unsigned int y = 0; y < gridBounds.CHUNK_MAX_Y_SIZE; y++) {
        for (unsigned int z = 0; z < gridBounds.usedChunkGridWidth; z++) {
            glm::ivec3 chunkPosition = glm::ivec3(borderX, y, z);
            if (isChunkInFrustum(chunkPosition, frustum, gridBounds, converter)) { 
                m_stack.emplace(std::make_pair(chunkPosition, ChunkVisibilityState::Side::NONE));
            }
        }
    }

    unsigned int borderY = 0;
    if (cameraPosition.y > 0) borderY = gridBounds.CHUNK_MAX_Y_SIZE - 1;

    for (unsigned int x = 0; x < gridBounds.usedChunkGridWidth; x++) {
        for (unsigned int z = 0; z < gridBounds.usedChunkGridWidth; z++) {
            glm::ivec3 chunkPosition = glm::ivec3(x, borderY, z);
            if (isChunkInFrustum(chunkPosition, frustum, gridBounds, converter)) { 
                m_stack.emplace(std::make_pair(chunkPosition, ChunkVisibilityState::Side::NONE));
            }
        }
    }

    unsigned int borderZ = 0;
    if (cameraPosition.z > gridBounds.currentCenterChunk.y * gridBounds.CHUNCK_DIMENSION_SIZE) borderZ = gridBounds.usedChunkGridWidth - 1;

    for (unsigned int y = 0; y < gridBounds.CHUNK_MAX_Y_SIZE; y++) {
        for (unsigned int x = 0; x < gridBounds.usedChunkGridWidth; x++) {
            glm::ivec3 chunkPosition = glm::ivec3(x, y, borderZ);
            if (isChunkInFrustum(chunkPosition, frustum, gridBounds, converter)) { 
                m_stack.emplace(std::make_pair(chunkPosition, ChunkVisibilityState::Side::NONE));
            }
        }
    }
}

std::array<std::pair<glm::ivec3, engine::ChunkVisibilityState::Side>, 7> engine::ChunkGridVisibility::findNeighbours(
    const glm::ivec3 &position, const ChunkGridBounds &gridBounds, bool allowGoUp, bool allowGoDown, bool allowGoFront, bool allowGoBack, bool allowGoRight, bool allowGoLeft
) {
    int usedChunkGridWidth = (int)gridBounds.usedChunkGridWidth;

    bool isPositiveXInbounds = (position.x + 1 < usedChunkGridWidth) && allowGoRight;
    bool isNegativeXInbounds = (position.x - 1 >= 0) && allowGoLeft;
    bool isPositiveYInbounds = (position.y + 1 < (int)gridBounds.CHUNK_MAX_Y_SIZE) && allowGoUp;
    bool isNegativeYInbounds = (position.y - 1 >= 0) && allowGoDown;
    bool isPositiveZInbounds = (position.z + 1 < usedChunkGridWidth) && allowGoFront;
    bool isNegativeZInbounds = (position.z - 1 >= 0) && allowGoBack;

    std::array<std::pair<glm::ivec3, ChunkVisibilityState::Side>, 7> neighbours;
    int cursor = 0;

    if (isPositiveXInbounds) {
        unsigned int x = position.x + 1;
        neighbours[cursor] = std::make_pair(glm::ivec3(x, position.y, position.z), ChunkVisibilityState::Side::RIGHT_FACE);
        cursor++;
    }
    if (isNegativeXInbounds) {
        unsigned int x = position.x - 1;
        neighbours[cursor] = std::make_pair(glm::ivec3(x, position.y, position.z), ChunkVisibilityState::Side::LEFT_FACE);
        cursor++;
    }
    if (isPositiveYInbounds) {
        unsigned int y = position.y + 1;
        neighbours[cursor] = std::make_pair(glm::ivec3(position.x, y, position.z), ChunkVisibilityState::Side::TOP_FACE);
        cursor++;
    }
    if (isNegativeYInbounds) {
        unsigned int y = position.y - 1;
        neighbours[cursor] = std::make_pair(glm::ivec3(position.x, y, position.z), ChunkVisibilityState::Side::BOTTOM_FACE);
        cursor++;
    }
    if (isPositiveZInbounds) {
        unsigned int z = position.z + 1;
        neighbours[cursor] = std::make_pair(glm::ivec3(position.x, position.y, z), ChunkVisibilityState::Side::FRONT_FACE);
        cursor++;
    }
    if (isNegativeZInbounds) {
        unsigned int z = position.z - 1;
        neighbours[cursor] = std::make_pair(glm::ivec3(position.x, position.y, z), ChunkVisibilityState::Side::BACK_FACE);
        cursor++;
    }

    neighbours[cursor].second = ChunkVisibilityState::Side::NONE;

    return neighbours;
}

engine::ChunkVisibilityState::Side engine::ChunkGridVisibility::getMirroredVisibilitySide(ChunkVisibilityState::Side side) {
    constexpr std::array<ChunkVisibilityState::Side, 27> mirroredSides = [](){
        std::array<ChunkVisibilityState::Side, 27> mirroredSides;
        for (int i = 0; i < 7; i++) {
            ChunkVisibilityState::Side side = static_cast<ChunkVisibilityState::Side>(i);
            ChunkVisibilityState::Side mirroredSide;
            switch (side) {
                case ChunkVisibilityState::Side::LEFT_FACE: mirroredSide = ChunkVisibilityState::Side::RIGHT_FACE; break;
                case ChunkVisibilityState::Side::RIGHT_FACE: mirroredSide = ChunkVisibilityState::Side::LEFT_FACE; break;
                case ChunkVisibilityState::Side::TOP_FACE: mirroredSide = ChunkVisibilityState::Side::BOTTOM_FACE; break;
                case ChunkVisibilityState::Side::BOTTOM_FACE: mirroredSide = ChunkVisibilityState::Side::TOP_FACE; break;
                case ChunkVisibilityState::Side::FRONT_FACE: mirroredSide = ChunkVisibilityState::Side::BACK_FACE; break;
                case ChunkVisibilityState::Side::BACK_FACE: mirroredSide = ChunkVisibilityState::Side::FRONT_FACE; break;
                case ChunkVisibilityState::Side::NONE: mirroredSide = ChunkVisibilityState::Side::NONE; break;
            }
            mirroredSides[i] = mirroredSide;
        }
        return mirroredSides;
    }();

    return mirroredSides[static_cast<int>(side)];
}
