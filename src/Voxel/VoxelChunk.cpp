#include "VoxelChunk.h"

engine::VoxelChunk::VoxelChunk() {}

engine::VoxelChunk::~VoxelChunk() {}

engine::MarchingCubesVoxel engine::VoxelChunk::getVoxel(short x, short y, short z) {
    return m_voxels(x, y, z);
}

bool engine::VoxelChunk::isVoxelSolid(short x, short y, short z) {
    uint8_t id = m_voxels(x, y, z).id;
    return id < 128;
}

void engine::VoxelChunk::setVoxel(short x, short y, short z, uint8_t id, uint8_t size) {
    m_voxels(x, y, z).id = id;
    m_voxels(x, y, z).size = size < 8 ? size : 7;
}

void engine::VoxelChunk::clear() {
    for (size_t x = 0; x < VOXEL_CHUNK_SIZE; x++) {
        for (size_t y = 0; y < VOXEL_CHUNK_SIZE; y++) {
            for (size_t z = 0; z < VOXEL_CHUNK_SIZE; z++) {
                m_voxels(x, y, z) = MarchingCubesVoxel();
            }
        }
    }
}

void engine::VoxelChunk::bindSSBO(GLuint index) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_ssbo);
}

void engine::VoxelChunk::addDrawCommand(const engine::DrawArraysIndirectCommand &command) {
    assert(m_drawCount < static_cast<int>(m_drawCommands.size()) && "ASSERT ERROR! - Adding more commands in chunk buffer than it can store!");
    m_drawCommands[m_drawCount] = command;
    m_drawCount++;
}

void engine::VoxelChunk::updateVisibilityStates() {
    VisibilityCheckState state[VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE];
    for (size_t x = 0; x < VOXEL_CHUNK_SIZE; x++) {
        for (size_t y = 0; y < VOXEL_CHUNK_SIZE; y++) {
            for (size_t z = 0; z < VOXEL_CHUNK_SIZE; z++) {
                state[x][y][z] = VisibilityCheckState::NOT_CHECKED;
            }
        }
    }

    clearVisibilityStates();

    // Проверка верхнего и нижнего края
    for (size_t x = 0; x < VOXEL_CHUNK_SIZE; x++) {
        for (size_t z = 0; z < VOXEL_CHUNK_SIZE; z++) {
            ChunkVisibilityState visibilityState;
            int edgeApproachCounter = floodFill(x, 0, z, state, visibilityState);
            if (edgeApproachCounter > 0) saveVisibilityStates(visibilityState);

            visibilityState.clear();
            edgeApproachCounter = floodFill(x, VOXEL_CHUNK_SIZE - 1, z, state, visibilityState);
            if (edgeApproachCounter > 0) saveVisibilityStates(visibilityState);
        }
    }
    // Проверка левого и правого края
    for (size_t y = 1; y < VOXEL_CHUNK_SIZE - 1; y++) {
        for (size_t z = 0; z < VOXEL_CHUNK_SIZE; z++) {
            ChunkVisibilityState visibilityState;
            int edgeApproachCounter = floodFill(0, y, z, state, visibilityState);
            if (edgeApproachCounter > 0) saveVisibilityStates(visibilityState);

            visibilityState.clear();
            edgeApproachCounter = floodFill(VOXEL_CHUNK_SIZE - 1, y, z, state, visibilityState);
            if (edgeApproachCounter > 0) saveVisibilityStates(visibilityState);
        }
    }
    // Проверка переднего и заднего края
    for (size_t x = 1; x < VOXEL_CHUNK_SIZE - 1; x++) {
        for (size_t y = 1; y < VOXEL_CHUNK_SIZE - 1; y++) {
            ChunkVisibilityState visibilityState;
            int edgeApproachCounter = floodFill(x, y, 0, state, visibilityState);
            if (edgeApproachCounter > 0) saveVisibilityStates(visibilityState);

            visibilityState.clear();
            edgeApproachCounter = floodFill(x, y, VOXEL_CHUNK_SIZE - 1, state, visibilityState);
            if (edgeApproachCounter > 0) saveVisibilityStates(visibilityState);
        }
    }
}

void engine::VoxelChunk::updateVisibilityStatesForEmptyChunk() {
    constexpr uint32_t SIDES_AND_NONE = ChunkVisibilityState::getSidesCount() + 1;
    for (size_t i = 0; i < SIDES_AND_NONE; i++) { 
        m_visibilityStates[i].setAllVisible(); 
    }
}

void engine::VoxelChunk::initSSBO() {
    glCreateBuffers(1, &m_ssbo);
    glNamedBufferData(m_ssbo, VOXEL_CHUNK_BYTE_SIZE, NULL, GL_DYNAMIC_DRAW);
}

void engine::VoxelChunk::freeSSBO() {
    glDeleteBuffers(1, &m_ssbo);
}

int engine::VoxelChunk::floodFill(
    short x, short y, short z, VisibilityCheckState (&state)[VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE], ChunkVisibilityState &visabilityState)
{
    if (!isVoxelEmptyAndNotChecked(x, y, z, state)) {
        return 0;
    }

    int edgeApproachCounter = 0;

    std::stack<glm::ivec3> stack;
    stack.push(glm::ivec3(x, y, z));

    while (!stack.empty()) {
        glm::ivec3 current = stack.top();
        stack.pop(); 

        int lx = current.x;
        while (isVoxelEmptyAndNotChecked(lx - 1, current.y, current.z, state) && lx > 0) {
            state[lx - 1][current.y][current.z] = VisibilityCheckState::CHECKED;
            lx--;
        }
        while (isVoxelEmptyAndNotChecked(current.x, current.y, current.z, state) && current.x < (int)VOXEL_CHUNK_SIZE) {
            state[current.x][current.y][current.z] = VisibilityCheckState::CHECKED;
            current.x++;
        }


            
        // Проверка краев
        ChunkVisibilityState currentPositionState;
        // Для X не должно быть else, потому что lx(левый) и x(правый) определяются в одной и той же итерации цикла
        if (current.x >= (int)VOXEL_CHUNK_SIZE) { 
            currentPositionState.set(ChunkVisibilityState::Side::RIGHT_FACE); edgeApproachCounter++; 
        }
        if (lx == 0) { 
            currentPositionState.set(ChunkVisibilityState::Side::LEFT_FACE); edgeApproachCounter++; 
        }

        if (current.y == (int)VOXEL_CHUNK_SIZE - 1) { currentPositionState.set(ChunkVisibilityState::Side::TOP_FACE); edgeApproachCounter++; }
        else if (current.y == 0)                    { currentPositionState.set(ChunkVisibilityState::Side::BOTTOM_FACE); edgeApproachCounter++; }
        
        if (current.z == (int)VOXEL_CHUNK_SIZE - 1) { currentPositionState.set(ChunkVisibilityState::Side::FRONT_FACE); edgeApproachCounter++; }
        else if (current.z == 0)                    { currentPositionState.set(ChunkVisibilityState::Side::BACK_FACE); edgeApproachCounter++; }

        visabilityState.merge(currentPositionState);



        lx = (lx - 1 < 0) ? 0 : lx - 1;
        current.x = (current.x + 1 >= (int)VOXEL_CHUNK_SIZE) ? (int)VOXEL_CHUNK_SIZE - 1 : current.x + 1; //
        bool yTopInside = current.y + 1 < (int)VOXEL_CHUNK_SIZE;
        bool yBottomInside = current.y - 1 >= 0;
        bool zFrontInside = current.z + 1 <(int)VOXEL_CHUNK_SIZE;
        bool zBackInside = current.z - 1 >= 0;
        if (yTopInside) {
            floodFillScanNext(lx, current.x, current.y + 1, current.z, stack, state);
            if (zFrontInside) floodFillScanNext(lx, current.x, current.y + 1, current.z + 1, stack, state);
            if (zBackInside) floodFillScanNext(lx, current.x, current.y + 1, current.z - 1, stack, state);
        }
        if (yBottomInside) {
            floodFillScanNext(lx, current.x, current.y - 1, current.z, stack, state);
            if (zFrontInside) floodFillScanNext(lx, current.x, current.y - 1, current.z + 1, stack, state);
            if (zBackInside) floodFillScanNext(lx, current.x, current.y - 1, current.z - 1, stack, state);
        }
        if (zFrontInside) floodFillScanNext(lx, current.x, current.y, current.z + 1, stack, state);
        if (zBackInside) floodFillScanNext(lx, current.x, current.y, current.z - 1, stack, state);
    }

    return edgeApproachCounter;
}

void engine::VoxelChunk::floodFillScanNext(
    int lx, int rx, int y, int z, std::stack<glm::ivec3> &stack, VisibilityCheckState (&state)[VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE]
) {
    bool spanAdded = false;
    for (int x = lx; x <= rx; x++) {
        if (!isVoxelEmptyAndNotChecked(x, y, z, state)) {
            spanAdded = false;
        }
        else if (!spanAdded) {
            stack.push(glm::ivec3(x, y, z));
            spanAdded = true;
        }
    }
}

void engine::VoxelChunk::saveVisibilityStates(ChunkVisibilityState& visibilityStates) {
    constexpr uint32_t sidesCount = ChunkVisibilityState::getSidesCount();
    for (uint32_t i = 0; i < sidesCount; i++) {
        if (visibilityStates.isVisible(i)) {
            for (uint32_t j = 0; j < sidesCount; j++) {
                if (visibilityStates.isVisible(j)) {
                    m_visibilityStates[i].set(j);
                    m_visibilityStates[j].set(i);
                }
            }
        }
    }
    m_visibilityStates[(int)ChunkVisibilityState::Side::NONE].merge(visibilityStates);
}

void engine::VoxelChunk::clearVisibilityStates() {
    constexpr uint32_t SIDES_AND_NONE = ChunkVisibilityState::getSidesCount() + 1;
    for (size_t i = 0; i < SIDES_AND_NONE; i++) { 
        m_visibilityStates[i].clear();  
    }
}

/*
*   0 - 1   4 - 5
*   |   |   |   |
*   3 - 2   7 - 6
*    Top    Bottom
*/
/*uint8_t engine::MarchingCube::getCombinationId() {
    uint8_t result = static_cast<uint8_t>(voxelTopFrontLeft->isSolid);
    result = result | (voxelTopFrontRight->isSolid << 1);
    result = result | (voxelTopBackRight->isSolid << 2);
    result = result | (voxelTopBackLeft->isSolid << 3);
    result = result | (voxelBottomFrontLeft->isSolid << 4);
    result = result | (voxelBottomFrontRight->isSolid << 5);
    result = result | (voxelBottomBackRight->isSolid << 6);
    result = result | (voxelBottomBackLeft->isSolid << 7);
    return result;
}*/
