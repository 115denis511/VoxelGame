#include "MarchingCubesSolver.h"

engine::MarchingCubesSolver::MarchingCubesSolver() {
    clear();
}

void engine::MarchingCubesSolver::regenerateChunk(
    MarchingCubes &marchingCubes, 
    ChunkGrid& grid, 
    glm::ivec3 position, 
    VoxelChunk &chunk, 
    ShaderStorageBuffer<glm::ivec2>& globalChunkSSBO
) {
    accumulateCases(marchingCubes, grid, position, chunk);

    // Сборка коммандного буфера
    chunk.clearDrawCommands();
    std::vector<glm::ivec2> dataBuffer;
    dataBuffer.reserve(m_cubesCount);
    int baseIndex = 0;
    for (int i = 1; i < 255; i++) {
        if (m_caseData[i].size() == 0) continue;
        dataBuffer.insert(dataBuffer.end(), m_caseData[i].begin(), m_caseData[i].end());
        DrawArraysIndirectCommand command = marchingCubes.getCommandTemplate(i);
        command.instanceCount = m_caseData[i].size();
        command.baseInstance = baseIndex;
        baseIndex += m_caseData[i].size();
        chunk.addDrawCommand(command);
    }

    if (chunk.getDrawCommandsCount() != 0) {
        chunk.updateVisibilityStates();
    }
    else {
        chunk.updateVisibilityStatesForEmptyChunk();
    }

    if (globalChunkSSBO.isInited()) {
        constexpr GLsizei dataPerChunk = VoxelChunk::VOXEL_CHUNK_BYTE_SIZE;
        GLsizei dataOffset = dataPerChunk * chunk.getIdInSSBO();
        globalChunkSSBO.pushData(&dataBuffer[0], dataBuffer.size(), dataOffset);
    }
    else {
        chunk.getSSBO().pushData(&dataBuffer[0], dataBuffer.size());
    }

    int errors = 0;
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        errors++;
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << "MarchingCubesSolver: " << error << std::endl;
    }

    clear();
}

void engine::MarchingCubesSolver::accumulateCases(MarchingCubes &marchingCubes, ChunkGrid &grid, glm::ivec3 position, VoxelChunk &chunk) {
    constexpr int MARCHING_CUBES_COUNT = 31;

    // Определение количества отрисовок для каждой из фигур марширующих кубов и
    // упаковка данных фигур для последующей загрузки в буфер SSBO
    for (int z = 0; z < MARCHING_CUBES_COUNT; z++) {
        for (int y = 0; y < MARCHING_CUBES_COUNT; y++) {
            for (int x = 0; x < MARCHING_CUBES_COUNT; x++) {
                std::array<MarchingCubesVoxel, 8> voxels {
                    chunk.getVoxel(x, y + 1, z + 1), // topFrontLeft
                    chunk.getVoxel(x + 1, y + 1, z + 1), // topFrontRight
                    chunk.getVoxel(x + 1, y + 1, z), // topBackRight
                    chunk.getVoxel(x, y + 1, z), // topBackLeft
                    chunk.getVoxel(x, y, z + 1), // bottomFrontLeft
                    chunk.getVoxel(x + 1, y, z + 1), // bottomFrontRight
                    chunk.getVoxel(x + 1, y, z), // bottomBackRight
                    chunk.getVoxel(x, y, z) // bottomBackLeft
                };
                addMarchingCube(marchingCubes, voxels, x, y, z);
            }
        }
    }

    ChunkGridBounds& gridBounds = grid.getGridBounds();
    VoxelPositionConverter& converter = grid.getPositionConverter();

    glm::ivec2 frontPos = converter.worldChunkToLocalChunkPosition(position.x, position.z + 1, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
    bool isFrontAvailable = gridBounds.isWorldChunkInbounds(position.x, position.y, position.z + 1) && grid.isHaveChunk(frontPos.x, position.y, frontPos.y);
    if (isFrontAvailable) {
        VoxelChunk& neighbour = grid.getChunk(frontPos.x, position.y, frontPos.y); 

        for (int y = 0; y < MARCHING_CUBES_COUNT; y++) {
            for (int x = 0; x < MARCHING_CUBES_COUNT; x++) {
                std::array<MarchingCubesVoxel, 8> voxels {
                    neighbour.getVoxel(x, y + 1, 0), // topFrontLeft
                    neighbour.getVoxel(x + 1, y + 1, 0), // topFrontRight
                    chunk.getVoxel(x + 1, y + 1, 31), // topBackRight
                    chunk.getVoxel(x, y + 1, 31), // topBackLeft
                    neighbour.getVoxel(x, y, 0), // bottomFrontLeft
                    neighbour.getVoxel(x + 1, y, 0), // bottomFrontRight
                    chunk.getVoxel(x + 1, y, 31), // bottomBackRight
                    chunk.getVoxel(x, y, 31) // bottomBackLeft
                };
                addMarchingCube(marchingCubes, voxels, x, y, 31);
            }
        }
    }

    glm::ivec2 rightPos = converter.worldChunkToLocalChunkPosition(position.x + 1, position.z, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
    bool isRightAvailable = gridBounds.isWorldChunkInbounds(position.x + 1, position.y, position.z) && grid.isHaveChunk(rightPos.x, position.y, rightPos.y);
    if (isRightAvailable) {
        VoxelChunk& neighbour = grid.getChunk(rightPos.x, position.y, rightPos.y); 

        for (int y = 0; y < MARCHING_CUBES_COUNT; y++) {
            for (int z = 0; z < MARCHING_CUBES_COUNT; z++) {
                std::array<MarchingCubesVoxel, 8> voxels {
                    chunk.getVoxel(31, y + 1, z + 1), // topFrontLeft
                    neighbour.getVoxel(0, y + 1, z + 1), // topFrontRight
                    neighbour.getVoxel(0, y + 1, z), // topBackRight
                    chunk.getVoxel(31, y + 1, z), // topBackLeft
                    chunk.getVoxel(31, y, z + 1), // bottomFrontLeft
                    neighbour.getVoxel(0, y, z + 1), // bottomFrontRight
                    neighbour.getVoxel(0, y, z), // bottomBackRight
                    chunk.getVoxel(31, y, z) // bottomBackLeft
                };
                addMarchingCube(marchingCubes, voxels, 31, y, z);
            }
        }
    }

    glm::ivec2 topPos = converter.worldChunkToLocalChunkPosition(position.x, position.z, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
    bool isUpAvailable = (position.y + 1 < gridBounds.CHUNK_MAX_Y_SIZE) && grid.isHaveChunk(topPos.x, position.y + 1, topPos.y);
    if (isUpAvailable) {
        VoxelChunk& neighbour = grid.getChunk(topPos.x, position.y + 1, topPos.y); 

        for (int x = 0; x < MARCHING_CUBES_COUNT; x++) {
            for (int z = 0; z < MARCHING_CUBES_COUNT; z++) {
                std::array<MarchingCubesVoxel, 8> voxels {
                    neighbour.getVoxel(x, 0, z + 1), // topFrontLeft
                    neighbour.getVoxel(x + 1, 0, z + 1), // topFrontRight
                    neighbour.getVoxel(x + 1, 0, z), // topBackRight
                    neighbour.getVoxel(x, 0, z), // topBackLeft
                    chunk.getVoxel(x, 31, z + 1), // bottomFrontLeft
                    chunk.getVoxel(x + 1, 31, z + 1), // bottomFrontRight
                    chunk.getVoxel(x + 1, 31, z), // bottomBackRight
                    chunk.getVoxel(x, 31, z) // bottomBackLeft
                };
                addMarchingCube(marchingCubes, voxels, x, 31, z);
            }
        }
    }

    glm::ivec2 frontRightPos = converter.worldChunkToLocalChunkPosition(position.x + 1, position.z + 1, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
    bool isFrontRightAvailable = isFrontAvailable && isRightAvailable && grid.isHaveChunk(frontRightPos.x, position.y, frontRightPos.y);
    if (isFrontRightAvailable) {
        VoxelChunk& neighbourFront = grid.getChunk(frontPos.x, position.y, frontPos.y); 
        VoxelChunk& neighbourRight = grid.getChunk(rightPos.x, position.y, rightPos.y); 
        VoxelChunk& neighbour = grid.getChunk(frontRightPos.x, position.y, frontRightPos.y); 

        for (int y = 0; y < MARCHING_CUBES_COUNT; y++) {
            std::array<MarchingCubesVoxel, 8> voxels {
                neighbourFront.getVoxel(31, y + 1, 0), // topFrontLeft
                neighbour.getVoxel(0, y + 1, 0), // topFrontRight
                neighbourRight.getVoxel(0, y + 1, 31), // topBackRight
                chunk.getVoxel(31, y + 1, 31), // topBackLeft
                neighbourFront.getVoxel(31, y, 0), // bottomFrontLeft
                neighbour.getVoxel(0, y, 0), // bottomFrontRight
                neighbourRight.getVoxel(0, y, 31), // bottomBackRight
                chunk.getVoxel(31, y, 31) // bottomBackLeft
            };
            addMarchingCube(marchingCubes, voxels, 31, y, 31);
        }
    }

    glm::ivec2 frontTopPos = converter.worldChunkToLocalChunkPosition(position.x, position.z + 1, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
    bool isFrontTopAvailable = isFrontAvailable && isUpAvailable && grid.isHaveChunk(frontTopPos.x, position.y + 1, frontTopPos.y);
    if (isFrontTopAvailable) {
        VoxelChunk& neighbourFront = grid.getChunk(frontPos.x, position.y, frontPos.y); 
        VoxelChunk& neighbourUp = grid.getChunk(topPos.x, position.y + 1, topPos.y); 
        VoxelChunk& neighbour = grid.getChunk(frontTopPos.x, position.y + 1, frontTopPos.y); 

        for (int x = 0; x < MARCHING_CUBES_COUNT; x++) {
            std::array<MarchingCubesVoxel, 8> voxels {
                neighbour.getVoxel(x, 0, 0), // topFrontLeft
                neighbour.getVoxel(x + 1, 0, 0), // topFrontRight
                neighbourUp.getVoxel(x + 1, 0, 31), // topBackRight
                neighbourUp.getVoxel(x, 0, 31), // topBackLeft
                neighbourFront.getVoxel(x, 31, 0), // bottomFrontLeft
                neighbourFront.getVoxel(x + 1, 31, 0), // bottomFrontRight
                chunk.getVoxel(x + 1, 31, 31), // bottomBackRight
                chunk.getVoxel(x, 31, 31) // bottomBackLeft
            };
            addMarchingCube(marchingCubes, voxels, x, 31, 31);
        }
    }

    glm::ivec2 rightTopPos = converter.worldChunkToLocalChunkPosition(position.x + 1, position.z, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
    bool isRightTopAvailable = isRightAvailable && isUpAvailable && grid.isHaveChunk(rightTopPos.x, position.y + 1, rightTopPos.y);
    if (isRightTopAvailable) {
        VoxelChunk& neighbourRight = grid.getChunk(rightPos.x, position.y, rightPos.y); 
        VoxelChunk& neighbourUp = grid.getChunk(topPos.x, position.y + 1, topPos.y); 
        VoxelChunk& neighbour = grid.getChunk(rightTopPos.x, position.y + 1, rightTopPos.y); 

        for (int z = 0; z < MARCHING_CUBES_COUNT; z++) {
            std::array<MarchingCubesVoxel, 8> voxels {
                neighbourUp.getVoxel(31, 0, z + 1), // topFrontLeft
                neighbour.getVoxel(0, 0, z + 1), // topFrontRight
                neighbour.getVoxel(0, 0, z), // topBackRight
                neighbourUp.getVoxel(31, 0, z), // topBackLeft
                chunk.getVoxel(31, 31, z + 1), // bottomFrontLeft
                neighbourRight.getVoxel(0, 31, z + 1), // bottomFrontRight
                neighbourRight.getVoxel(0, 31, z), // bottomBackRight
                chunk.getVoxel(31, 31, z) // bottomBackLeft
            };
            addMarchingCube(marchingCubes, voxels, 31, 31, z);
        }
    }

    glm::ivec2 frontRightTopPos = converter.worldChunkToLocalChunkPosition(position.x + 1, position.z + 1, gridBounds.currentOriginChunk.x, gridBounds.currentOriginChunk.y);
    if (isFrontRightAvailable && isFrontTopAvailable && isRightTopAvailable && grid.isHaveChunk(frontRightTopPos.x, position.y + 1, frontRightTopPos.y)) {
        VoxelChunk& neighbourFront = grid.getChunk(frontPos.x, position.y, frontPos.y); 
        VoxelChunk& neighbourRight = grid.getChunk(rightPos.x, position.y, rightPos.y); 
        VoxelChunk& neighbourUp = grid.getChunk(topPos.x, position.y + 1, topPos.y); 
        VoxelChunk& neighbourFrontRight = grid.getChunk(frontRightPos.x, position.y, frontRightPos.y); 
        VoxelChunk& neighbourFrontTop = grid.getChunk(frontTopPos.x, position.y + 1, frontTopPos.y); 
        VoxelChunk& neighbourRightTop = grid.getChunk(rightTopPos.x, position.y + 1, rightTopPos.y); 
        VoxelChunk& neighbourFrontRightTop = grid.getChunk(frontRightTopPos.x, position.y + 1, frontRightTopPos.y); 

        std::array<MarchingCubesVoxel, 8> voxels {
            neighbourFrontTop.getVoxel(31, 0, 0), // topFrontLeft
            neighbourFrontRightTop.getVoxel(0, 0, 0), // topFrontRight
            neighbourRightTop.getVoxel(0, 0, 31), // topBackRight
            neighbourUp.getVoxel(31, 0, 31), // topBackLeft
            neighbourFront.getVoxel(31, 31, 0), // bottomFrontLeft
            neighbourFrontRight.getVoxel(0, 31, 0), // bottomFrontRight
            neighbourRight.getVoxel(0, 31, 31), // bottomBackRight
            chunk.getVoxel(31, 31, 31) // bottomBackLeft
        };
        addMarchingCube(marchingCubes, voxels, 31, 31, 31);
    }
}

void engine::MarchingCubesSolver::clear() {
    m_cubesCount = 0;

    for (std::vector<glm::ivec2>& caseBuffer : m_caseData) {
        caseBuffer.clear();
    }
}

void engine::MarchingCubesSolver::addMarchingCube(MarchingCubes& marchingCubes, std::array<MarchingCubesVoxel, 8> &voxels, int x, int y, int z) {
    uint8_t caseId = getCaseIndex(voxels);
    if (caseId == 0 || caseId == 255) return;

    auto verticesIds = marchingCubes.getVertecesIds(caseId);
    // vId - индекс вершины в массиве voxels[]
    int offsets[6] = { 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 6; i++) {
        int vId = verticesIds.ids[i];
        offsets[i] = voxels[vId].size;
    }
    int textures[4] = { 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++) {
        int vId = verticesIds.ids[i];
        textures[i] = voxels[vId].id;
    }

    glm::ivec2 caseData = packData(x, y, z, offsets, textures);
    m_caseData[caseId].push_back(caseData);
    m_cubesCount++;
}

uint8_t engine::MarchingCubesSolver::getCaseIndex(std::array<MarchingCubesVoxel, 8>& voxels) {
    uint8_t caseId = voxels[0].id < 128;
    caseId |= (voxels[1].id < 128) << 1;
    caseId |= (voxels[2].id < 128) << 2;
    caseId |= (voxels[3].id < 128) << 3;
    caseId |= (voxels[4].id < 128) << 4;
    caseId |= (voxels[5].id < 128) << 5;
    caseId |= (voxels[6].id < 128) << 6;
    caseId |= (voxels[7].id < 128) << 7;
    return caseId;
}

glm::ivec2 engine::MarchingCubesSolver::packData(int x, int y, int z, int offset[6], int textureId[4]) {
    int left = x;
    left <<= 5;
    left |= y;
    left <<= 5;
    left |= z;
    left <<= 3;
    left |= offset[0]; // 1
    left <<= 3;
    left |= offset[1]; // 2
    left <<= 3;
    left |= offset[2]; // 3
    left <<= 3;
    left |= offset[3]; // 4
    left <<= 3;
    left |= offset[4]; // 5
    int right = offset[5]; // 6
    right <<= 7;
    right |= textureId[0] & 0b1111111; // 1
    right <<= 7;
    right |= textureId[1] & 0b1111111; // 2
    right <<= 7;
    right |= textureId[2] & 0b1111111; // 3
    right <<= 7;
    right |= textureId[3] & 0b1111111; // 4

    return glm::ivec2(left, right);
}
