#include "ChunkVisibilityChecker.h"

void engine::ChunkVisibilityChecker::updateVisibilityStates(ChunkGrid &grid, VoxelChunk& chunk) {
    clearStates();
    chunk.clearVisibilityStates();

    // Проверка верхнего и нижнего края
    for (size_t x = 0; x < STATE_GRID_SIZE; x++) {
        for (size_t z = 0; z < STATE_GRID_SIZE; z++) {
            ChunkVisibilityState visibilityState;
            int edgeApproachCounter = floodFill(x, 0, z, chunk, visibilityState);
            if (edgeApproachCounter > 0) chunk.saveVisibilityStates(visibilityState);

            visibilityState.clear();
            edgeApproachCounter = floodFill(x, STATE_GRID_SIZE - 1, z, chunk, visibilityState);
            if (edgeApproachCounter > 0) chunk.saveVisibilityStates(visibilityState);
        }
    }
    // Проверка левого и правого края
    for (size_t y = 1; y < STATE_GRID_SIZE - 1; y++) {
        for (size_t z = 0; z < STATE_GRID_SIZE; z++) {
            ChunkVisibilityState visibilityState;
            int edgeApproachCounter = floodFill(0, y, z, chunk, visibilityState);
            if (edgeApproachCounter > 0) chunk.saveVisibilityStates(visibilityState);

            visibilityState.clear();
            edgeApproachCounter = floodFill(STATE_GRID_SIZE - 1, y, z, chunk, visibilityState);
            if (edgeApproachCounter > 0) chunk.saveVisibilityStates(visibilityState);
        }
    }
    // Проверка переднего и заднего края
    for (size_t x = 1; x < STATE_GRID_SIZE - 1; x++) {
        for (size_t y = 1; y < STATE_GRID_SIZE - 1; y++) {
            ChunkVisibilityState visibilityState;
            int edgeApproachCounter = floodFill(x, y, 0, chunk, visibilityState);
            if (edgeApproachCounter > 0) chunk.saveVisibilityStates(visibilityState);

            visibilityState.clear();
            edgeApproachCounter = floodFill(x, y, STATE_GRID_SIZE - 1, chunk, visibilityState);
            if (edgeApproachCounter > 0) chunk.saveVisibilityStates(visibilityState);
        }
    }
}

void engine::ChunkVisibilityChecker::clearStates() {
    m_state.fillAllRows(0);
}

int engine::ChunkVisibilityChecker::floodFill(int x, int y, int z, VoxelChunk& chunk, ChunkVisibilityState& visabilityState) {
    if (!isVoxelEmptyAndNotChecked(x, y, z, chunk)) { return 0; }

    int edgeApproachCounter = 0;
    auto& grid = chunk.getEmptyVoxelsGrid();

    m_state.set(x, y, z);
    uint32_t empty = grid.getRow(y, z);
    uint32_t orig = m_state.getRow(y, z);
    uint32_t cur = expandRow(orig, empty);
    if (cur == 0) return 0;
    m_state.setRow(y, z, cur);
    edgeApproachCounter += checkIsOnBorder(cur, glm::ivec2(y,z), visabilityState);

    m_stack.push(glm::ivec2(y, z));
    while (!m_stack.empty()) {
        auto current = m_stack.top();
        m_stack.pop();

        int curY = current.x;
        int curZ = current.y;
        
        bool yTopInside = curY + 1 < BinaryGrid<uint32_t>::SIZE;
        bool yBottomInside = curY - 1 >= 0;
        bool zFrontInside = curZ + 1 < BinaryGrid<uint32_t>::SIZE;
        bool zBackInside = curZ - 1 >= 0;
        
        uint32_t expand = m_state.getRow(curY, curZ);
        uint32_t l = expand << 1;
        uint32_t r = expand >> 1;
        expand |= l | r;

        if (yTopInside) {
            edgeApproachCounter += checkNeighbourRow(curY + 1, curZ, expand, grid, visabilityState);
            if (zFrontInside) edgeApproachCounter += checkNeighbourRow(curY + 1, curZ + 1, expand, grid, visabilityState);
            if (zBackInside) edgeApproachCounter += checkNeighbourRow(curY + 1, curZ - 1, expand, grid, visabilityState);
        }
        if (yBottomInside) {
            edgeApproachCounter += checkNeighbourRow(curY - 1, curZ, expand, grid, visabilityState);
            if (zFrontInside) edgeApproachCounter += checkNeighbourRow(curY - 1, curZ + 1, expand, grid, visabilityState);
            if (zBackInside) edgeApproachCounter += checkNeighbourRow(curY - 1, curZ - 1, expand, grid, visabilityState);
        }
        if (zFrontInside) edgeApproachCounter += checkNeighbourRow(curY, curZ + 1, expand, grid, visabilityState);
        if (zBackInside) edgeApproachCounter += checkNeighbourRow(curY, curZ - 1, expand, grid, visabilityState);
    }

    return edgeApproachCounter;
}

uint32_t engine::ChunkVisibilityChecker::expandRow(uint32_t orig, uint32_t empty) {
    orig &= empty;
    if (orig == 0) return 0;

    uint32_t cur = orig;
    uint32_t prev = 0;
    while (cur != prev) {
        prev = cur;

        uint32_t l = cur << 1;
        uint32_t r = cur >> 1;
        cur |= l | r;
        cur &= empty;
    }
    return cur;
}

int engine::ChunkVisibilityChecker::checkNeighbourRow(int y, int z, uint32_t expand, const BinaryGrid<uint32_t> &grid, ChunkVisibilityState& visabilityState) {
    int edgeApproachCounter = 0;

    uint32_t empty = grid.getRow(y, z);
    uint32_t orig = m_state.getRow(y, z);
    uint32_t cur = expandRow(orig | expand, empty);
    if (cur != 0 && cur != orig) {
        m_state.setRow(y, z, cur);
        edgeApproachCounter += checkIsOnBorder(cur, glm::ivec2(y, z), visabilityState);
        m_stack.push(glm::ivec2(y, z));
    }

    return edgeApproachCounter;
}

int engine::ChunkVisibilityChecker::checkIsOnBorder(uint32_t row, glm::ivec2 pos, ChunkVisibilityState &visabilityState) {
    int edgeApproachCounter = 0;
    ChunkVisibilityState currentPositionState;

    int y = pos.x;
    int z = pos.y;

    if (row & BinaryGrid<uint32_t>::LEFT_BIT) {
        currentPositionState.set(ChunkVisibilityState::Side::LEFT_FACE);
        edgeApproachCounter++;
    }
    if (row & BinaryGrid<uint32_t>::RIGHT_BIT) {
        currentPositionState.set(ChunkVisibilityState::Side::RIGHT_FACE);
        edgeApproachCounter++;
    }

    if (y == BinaryGrid<uint32_t>::SIZE - 1) { 
        currentPositionState.set(ChunkVisibilityState::Side::TOP_FACE); 
        edgeApproachCounter++; 
    }
    else if (y == 0) { 
        currentPositionState.set(ChunkVisibilityState::Side::BOTTOM_FACE); 
        edgeApproachCounter++; 
    }
    
    if (z == BinaryGrid<uint32_t>::SIZE - 1) { 
        currentPositionState.set(ChunkVisibilityState::Side::FRONT_FACE); 
        edgeApproachCounter++; 
    }
    else if (z == 0) { 
        currentPositionState.set(ChunkVisibilityState::Side::BACK_FACE); 
        edgeApproachCounter++; 
    }

    visabilityState.merge(currentPositionState);
    return edgeApproachCounter;
}
