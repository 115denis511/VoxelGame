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
    for (size_t x = 0; x < STATE_GRID_SIZE; x++) {
        for (size_t y = 0; y < STATE_GRID_SIZE; y++) {
            for (size_t z = 0; z < STATE_GRID_SIZE; z++) {
                m_state[x][y][z] = VisibilityCheckState::NOT_CHECKED;
            }
        }
    }
}

int engine::ChunkVisibilityChecker::floodFill(int x, int y, int z, VoxelChunk& chunk, ChunkVisibilityState &visabilityState) {
    if (!isVoxelEmptyAndNotChecked(x, y, z, chunk)) { return 0; }

    int edgeApproachCounter = 0;
    m_stack.push(glm::ivec3(x, y, z));
    
    while (!m_stack.empty()) {
        glm::ivec3 current = m_stack.top();
        m_stack.pop();

        // Центр
        m_state[current.x][current.y][current.z] = VisibilityCheckState::CHECKED; 
        // Обход влево
        int lx = current.x;
        while (lx > 0 && isVoxelEmptyAndNotChecked(lx - 1, current.y, current.z, chunk)) { 
            m_state[lx - 1][current.y][current.z] = VisibilityCheckState::CHECKED;
            lx--;
        }
        // Обход вправо
        while (current.x < STATE_GRID_SIZE - 1 && isVoxelEmptyAndNotChecked(current.x + 1, current.y, current.z, chunk) ) { 
            m_state[current.x + 1][current.y][current.z] = VisibilityCheckState::CHECKED;
            current.x++;
        }


        // Проверка краев
        ChunkVisibilityState currentPositionState;
        // Для X не должно быть else, потому что lx(левый) и x(правый) определяются в одной и той же итерации цикла
        if (current.x == STATE_GRID_SIZE - 1) { 
            currentPositionState.set(ChunkVisibilityState::Side::RIGHT_FACE); edgeApproachCounter++; 
        }
        if (lx == 0) { 
            currentPositionState.set(ChunkVisibilityState::Side::LEFT_FACE); edgeApproachCounter++; 
        }

        if (current.y == STATE_GRID_SIZE - 1) { currentPositionState.set(ChunkVisibilityState::Side::TOP_FACE); edgeApproachCounter++; }
        else if (current.y == 0) { currentPositionState.set(ChunkVisibilityState::Side::BOTTOM_FACE); edgeApproachCounter++; }
        
        if (current.z == STATE_GRID_SIZE - 1) { currentPositionState.set(ChunkVisibilityState::Side::FRONT_FACE); edgeApproachCounter++; }
        else if (current.z == 0) { currentPositionState.set(ChunkVisibilityState::Side::BACK_FACE); edgeApproachCounter++; }

        visabilityState.merge(currentPositionState);


        lx = (lx - 1 < 0) ? 0 : lx - 1;
        current.x = (current.x + 1 >= STATE_GRID_SIZE) ? STATE_GRID_SIZE - 1 : current.x + 1;
        bool yTopInside = current.y + 1 < STATE_GRID_SIZE;
        bool yBottomInside = current.y - 1 >= 0;
        bool zFrontInside = current.z + 1 < STATE_GRID_SIZE;
        bool zBackInside = current.z - 1 >= 0;
        if (yTopInside) {
            floodFillScanNext(lx, current.x, current.y + 1, current.z, chunk);
            if (zFrontInside) floodFillScanNext(lx, current.x, current.y + 1, current.z + 1, chunk);
            if (zBackInside) floodFillScanNext(lx, current.x, current.y + 1, current.z - 1, chunk);
        }
        if (yBottomInside) {
            floodFillScanNext(lx, current.x, current.y - 1, current.z, chunk);
            if (zFrontInside) floodFillScanNext(lx, current.x, current.y - 1, current.z + 1, chunk);
            if (zBackInside) floodFillScanNext(lx, current.x, current.y - 1, current.z - 1, chunk);
        }
        if (zFrontInside) floodFillScanNext(lx, current.x, current.y, current.z + 1, chunk);
        if (zBackInside) floodFillScanNext(lx, current.x, current.y, current.z - 1, chunk);
    }

    return edgeApproachCounter;
}

void engine::ChunkVisibilityChecker::floodFillScanNext(int lx, int rx, int y, int z, VoxelChunk& chunk) {
    bool spanAdded = false;
    for (int x = lx; x <= rx; x++) {
        if (!isVoxelEmptyAndNotChecked(x, y, z, chunk)) {
            spanAdded = false;
        }
        else if (!spanAdded) {
            m_stack.push(glm::ivec3(x, y, z));
            spanAdded = true;
        }
    }
}
