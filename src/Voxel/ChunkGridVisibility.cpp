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
    const Frustum& frustum, VisabilityType type, ChunkGrid &grid, const ChunkGridBounds &gridBounds
) {
    // Сделано на основе следующих материалов:
    // https://tomcc.github.io/2014/08/31/visibility-1.html
    // https://tomcc.github.io/2014/08/31/visibility-2.html
    glm::ivec3 cameraChunk = VoxelPositionConverter::worldPositionToChunkPosition(cameraPosition, gridBounds.CHUNCK_DIMENSION_SIZE);
    glm::ivec3 cameraChunkLocal = VoxelPositionConverter::worldChunkToLocalChunkPosition(cameraChunk, gridBounds.currentOriginChunk);

    if (gridBounds.usedChunkGridWidth == 0) return;
    if (!gridBounds.isChunkInbounds(cameraChunk.x, cameraChunk.y, cameraChunk.z)) {
        cameraChunkLocal = findClosestVisibleChunkForCameraOutsideBounds(
            cameraPosition, cameraDirection, frustum, gridBounds
        );
    }

    m_stack.push(std::make_pair(cameraChunkLocal, ChunkVisibilityState::Side::NONE));

    while (!m_stack.empty()) {
        glm::ivec3 currentChunk = m_stack.top().first;
        ChunkVisibilityState::Side cameFrom = getMirroredVisibilitySide(m_stack.top().second);
        m_stack.pop(); 

        if (isVisible(currentChunk.x, currentChunk.y, currentChunk.z, type)) { continue; }
        setVisibilityStateVisible(currentChunk, type);

        findNeighbours(
            currentChunk, gridBounds, 
            currentChunk.y >= cameraChunkLocal.y, currentChunk.y <= cameraChunkLocal.y,
            currentChunk.z >= cameraChunkLocal.z, currentChunk.z <= cameraChunkLocal.z,
            currentChunk.x >= cameraChunkLocal.x, currentChunk.x <= cameraChunkLocal.x
        );
        for (int i = 0; i < m_neighboursCount; i++) {
            glm::ivec3 neighbourChunk = m_neighbours[i].first;
            ChunkVisibilityState::Side cameTo = m_neighbours[i].second;

            if (isVisible(neighbourChunk.x, neighbourChunk.y, neighbourChunk.z, type)) { continue; }

            VoxelChunk& chunk = grid.getChunk(currentChunk.x, currentChunk.y, currentChunk.z);
            if (!chunk.isVisibleThrough(cameFrom, cameTo)) { continue; }

            if (!isChunkInFrustum(neighbourChunk, frustum, gridBounds)) { continue; }

            m_stack.push(std::pair<glm::ivec3, ChunkVisibilityState::Side>(neighbourChunk, cameTo));
        }
    }
}

bool engine::ChunkGridVisibility::isChunkInFrustum(const glm::ivec3 &chunkPosition, const Frustum &frustum, const ChunkGridBounds &gridBounds) {
    glm::ivec2 worldChunkPos = VoxelPositionConverter::localChunkToWorldChunkPosition(
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

glm::ivec3 engine::ChunkGridVisibility::findClosestVisibleChunkForCameraOutsideBounds(
    const glm::vec3 &cameraPosition, const glm::vec3& cameraDirection, const Frustum &frustum, const ChunkGridBounds &gridBounds
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
    glm::ivec3 iPosition = VoxelPositionConverter::worldPositionToChunkPosition(position, gridBounds.CHUNCK_DIMENSION_SIZE);
    iPosition = VoxelPositionConverter::worldChunkToLocalChunkPosition(iPosition.x, iPosition.y, iPosition.z, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);

    if (iPosition.x < 0) { iPosition.x = 0; }
    else if (iPosition.x >= (int)gridBounds.usedChunkGridWidth) { iPosition.x = (int)gridBounds.usedChunkGridWidth - 1; }
    if (iPosition.y < 0) { iPosition.y = 0; }
    else if (iPosition.y > (int)gridBounds.CHUNK_MAX_Y_SIZE) { iPosition.y = gridBounds.CHUNK_MAX_Y_SIZE - 1; }
    if (iPosition.z < 0) { iPosition.z = 0; }
    else if (iPosition.z >= (int)gridBounds.usedChunkGridWidth) { iPosition.z = (int)gridBounds.usedChunkGridWidth - 1; }

    return iPosition;
}

void engine::ChunkGridVisibility::findNeighbours(
    const glm::ivec3 &position, const ChunkGridBounds &gridBounds, bool allowGoUp, bool allowGoDown, bool allowGoFront, bool allowGoBack, bool allowGoRight, bool allowGoLeft
) {
    m_neighboursCount = 0;

    int usedChunkGridWidth = (int)gridBounds.usedChunkGridWidth;

    bool isPositiveXInbounds = (position.x + 1 < usedChunkGridWidth) && allowGoRight;
    bool isNegativeXInbounds = (position.x - 1 >= 0) && allowGoLeft;
    bool isPositiveYInbounds = (position.y + 1 < (int)gridBounds.CHUNK_MAX_Y_SIZE) && allowGoUp;
    bool isNegativeYInbounds = (position.y - 1 >= 0) && allowGoDown;
    bool isPositiveZInbounds = (position.z + 1 < usedChunkGridWidth) && allowGoFront;
    bool isNegativeZInbounds = (position.z - 1 >= 0) && allowGoBack;

    if (isPositiveYInbounds) {
        unsigned int y = position.y + 1;
        m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(position.x, y, position.z), ChunkVisibilityState::Side::TOP_FACE);
        m_neighboursCount++;

        if (isPositiveXInbounds) { 
            unsigned int x = position.x + 1;
            m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z), ChunkVisibilityState::Side::TOP_RIGHT_EDGE);
            m_neighboursCount++;

            if (isPositiveZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z + 1), ChunkVisibilityState::Side::RIGHT_TOP_FRONT_CORNER); m_neighboursCount++; }
            if (isNegativeZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z - 1), ChunkVisibilityState::Side::RIGHT_TOP_BACK_CORNER); m_neighboursCount++; }    
        }
        if (isNegativeXInbounds) {
            unsigned int x = position.x - 1;
            m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z), ChunkVisibilityState::Side::TOP_LEFT_EDGE);
            m_neighboursCount++;

            if (isPositiveZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z + 1), ChunkVisibilityState::Side::LEFT_TOP_FRONT_CORNER); m_neighboursCount++; }
            if (isNegativeZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z - 1), ChunkVisibilityState::Side::LEFT_TOP_BACK_CORNER); m_neighboursCount++; }
        }
        if (isPositiveZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(position.x, y, position.z + 1), ChunkVisibilityState::Side::TOP_FRONT_EDGE); m_neighboursCount++; }
        if (isNegativeZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(position.x, y, position.z - 1), ChunkVisibilityState::Side::TOP_BACK_EDGE); m_neighboursCount++; }
    }
    if (isNegativeYInbounds) {
        unsigned int y = position.y - 1;
        m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(position.x, y, position.z), ChunkVisibilityState::Side::BOTTOM_FACE);
        m_neighboursCount++;

        if (isPositiveXInbounds) {
            unsigned int x = position.x + 1;
            m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z), ChunkVisibilityState::Side::BOTTOM_RIGHT_EDGE);
            m_neighboursCount++;

            if (isPositiveZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z + 1), ChunkVisibilityState::Side::RIGHT_BOTTOM_FRONT_CORNER); m_neighboursCount++; }
            if (isNegativeZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z - 1), ChunkVisibilityState::Side::RIGHT_BOTTOM_BACK_CORNER); m_neighboursCount++; }
        }
        if (isNegativeXInbounds) {
            unsigned int x = position.x - 1;
            m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z), ChunkVisibilityState::Side::BOTTOM_LEFT_EDGE);
            m_neighboursCount++;

            if (isPositiveZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z + 1), ChunkVisibilityState::Side::LEFT_BOTTOM_FRONT_CORNER); m_neighboursCount++; }
            if (isNegativeZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, y, position.z - 1), ChunkVisibilityState::Side::LEFT_BOTTOM_BACK_CORNER); m_neighboursCount++; }
        }
        if (isPositiveZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(position.x, y, position.z + 1), ChunkVisibilityState::Side::BOTTOM_FRONT_EDGE); m_neighboursCount++; }
        if (isNegativeZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(position.x, y, position.z - 1), ChunkVisibilityState::Side::BOTTOM_BACK_EDGE); m_neighboursCount++; }
    }
    if (isPositiveXInbounds) {
        unsigned int x = position.x + 1;
        m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, position.y, position.z), ChunkVisibilityState::Side::RIGHT_FACE);
        m_neighboursCount++;

        if (isPositiveZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, position.y, position.z + 1), ChunkVisibilityState::Side::RIGHT_FRONT_EDGE); m_neighboursCount++; }
        if (isNegativeZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, position.y, position.z - 1), ChunkVisibilityState::Side::RIGHT_BACK_EDGE); m_neighboursCount++; }
    }
    if (isNegativeXInbounds) {
        unsigned int x = position.x - 1;
        m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, position.y, position.z), ChunkVisibilityState::Side::LEFT_FACE);
        m_neighboursCount++;

        if (isPositiveZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, position.y, position.z + 1), ChunkVisibilityState::Side::LEFT_FRONT_EDGE); m_neighboursCount++; }
        if (isNegativeZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(x, position.y, position.z - 1), ChunkVisibilityState::Side::LEFT_BACK_EDGE); m_neighboursCount++; }
    }
    if (isPositiveZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(position.x, position.y, position.z + 1), ChunkVisibilityState::Side::FRONT_FACE); m_neighboursCount++; }
    if (isNegativeZInbounds) { m_neighbours[m_neighboursCount] = std::make_pair(glm::ivec3(position.x, position.y, position.z - 1), ChunkVisibilityState::Side::BACK_FACE); m_neighboursCount++; }
}

engine::ChunkVisibilityState::Side engine::ChunkGridVisibility::getMirroredVisibilitySide(ChunkVisibilityState::Side side) {
    constexpr std::array<ChunkVisibilityState::Side, 27> mirroredSides = [](){
        std::array<ChunkVisibilityState::Side, 27> mirroredSides;
        for (int i = 0; i < 27; i++) {
            ChunkVisibilityState::Side side = static_cast<ChunkVisibilityState::Side>(i);
            ChunkVisibilityState::Side mirroredSide;
            switch (side) {
                case ChunkVisibilityState::Side::LEFT_FACE: mirroredSide = ChunkVisibilityState::Side::RIGHT_FACE; break;
                case ChunkVisibilityState::Side::RIGHT_FACE: mirroredSide = ChunkVisibilityState::Side::LEFT_FACE; break;
                case ChunkVisibilityState::Side::TOP_FACE: mirroredSide = ChunkVisibilityState::Side::BOTTOM_FACE; break;
                case ChunkVisibilityState::Side::BOTTOM_FACE: mirroredSide = ChunkVisibilityState::Side::TOP_FACE; break;
                case ChunkVisibilityState::Side::FRONT_FACE: mirroredSide = ChunkVisibilityState::Side::BACK_FACE; break;
                case ChunkVisibilityState::Side::BACK_FACE: mirroredSide = ChunkVisibilityState::Side::FRONT_FACE; break;
                case ChunkVisibilityState::Side::LEFT_BOTTOM_BACK_CORNER: mirroredSide = ChunkVisibilityState::Side::RIGHT_TOP_FRONT_CORNER; break;
                case ChunkVisibilityState::Side::RIGHT_BOTTOM_BACK_CORNER: mirroredSide = ChunkVisibilityState::Side::LEFT_TOP_FRONT_CORNER; break;
                case ChunkVisibilityState::Side::LEFT_BOTTOM_FRONT_CORNER: mirroredSide = ChunkVisibilityState::Side::RIGHT_TOP_BACK_CORNER; break;
                case ChunkVisibilityState::Side::RIGHT_BOTTOM_FRONT_CORNER: mirroredSide = ChunkVisibilityState::Side::LEFT_TOP_BACK_CORNER; break;
                case ChunkVisibilityState::Side::LEFT_TOP_BACK_CORNER: mirroredSide = ChunkVisibilityState::Side::RIGHT_BOTTOM_FRONT_CORNER; break;
                case ChunkVisibilityState::Side::RIGHT_TOP_BACK_CORNER: mirroredSide = ChunkVisibilityState::Side::LEFT_BOTTOM_FRONT_CORNER; break;
                case ChunkVisibilityState::Side::LEFT_TOP_FRONT_CORNER: mirroredSide = ChunkVisibilityState::Side::RIGHT_BOTTOM_BACK_CORNER; break;
                case ChunkVisibilityState::Side::RIGHT_TOP_FRONT_CORNER: mirroredSide = ChunkVisibilityState::Side::LEFT_BOTTOM_BACK_CORNER; break;
                case ChunkVisibilityState::Side::TOP_FRONT_EDGE: mirroredSide = ChunkVisibilityState::Side::BOTTOM_BACK_EDGE; break;
                case ChunkVisibilityState::Side::TOP_BACK_EDGE: mirroredSide = ChunkVisibilityState::Side::BOTTOM_FRONT_EDGE; break;
                case ChunkVisibilityState::Side::TOP_LEFT_EDGE: mirroredSide = ChunkVisibilityState::Side::BOTTOM_RIGHT_EDGE; break;
                case ChunkVisibilityState::Side::TOP_RIGHT_EDGE: mirroredSide = ChunkVisibilityState::Side::BOTTOM_LEFT_EDGE; break;
                case ChunkVisibilityState::Side::BOTTOM_FRONT_EDGE: mirroredSide = ChunkVisibilityState::Side::TOP_BACK_EDGE; break;
                case ChunkVisibilityState::Side::BOTTOM_BACK_EDGE: mirroredSide = ChunkVisibilityState::Side::TOP_FRONT_EDGE; break;
                case ChunkVisibilityState::Side::BOTTOM_LEFT_EDGE: mirroredSide = ChunkVisibilityState::Side::TOP_RIGHT_EDGE; break;
                case ChunkVisibilityState::Side::BOTTOM_RIGHT_EDGE: mirroredSide = ChunkVisibilityState::Side::TOP_LEFT_EDGE; break;
                case ChunkVisibilityState::Side::LEFT_FRONT_EDGE: mirroredSide = ChunkVisibilityState::Side::RIGHT_BACK_EDGE; break;
                case ChunkVisibilityState::Side::RIGHT_FRONT_EDGE: mirroredSide = ChunkVisibilityState::Side::LEFT_BACK_EDGE; break;
                case ChunkVisibilityState::Side::LEFT_BACK_EDGE: mirroredSide = ChunkVisibilityState::Side::RIGHT_FRONT_EDGE; break;
                case ChunkVisibilityState::Side::RIGHT_BACK_EDGE: mirroredSide = ChunkVisibilityState::Side::LEFT_FRONT_EDGE; break;
                case ChunkVisibilityState::Side::NONE: mirroredSide = ChunkVisibilityState::Side::NONE; break;
            }
            mirroredSides[i] = mirroredSide;
        }
        return mirroredSides;
    }();

    return mirroredSides[static_cast<int>(side)];
}
