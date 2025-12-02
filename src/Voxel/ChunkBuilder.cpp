#include "ChunkBuilder.h"

engine::ChunkBuilder::ChunkBuilder() {
    clear();
}

void engine::ChunkBuilder::regenerateChunk(
    MarchingCubes &marchingCubes, 
    ChunkGrid& grid,
    VoxelChunk &chunk, 
    ShaderStorageBuffer<GLuint>& globalChunkSSBO,
    ShaderStorageBuffer<Voxel>& globalChunkGridsSSBO
) {
    accumulateCases(marchingCubes, grid, chunk);

    // Сборка коммандного буфера
    chunk.clearDrawCommands();
    std::vector<GLuint> dataBuffer(std::max(m_solidCubesCount, m_liquidCubesCount)); // TODO: Данный буфер не должен пересоздаваться каждый вызов методв

    // Твёрдые воксели
    int baseIndex = 0;
    int dataIndex = 0;
    for (int i = 1; i < 255; i++) {
        if (m_solidCaseData[i].size() == 0) continue;

        for (GLuint caseData : m_solidCaseData[i]) {
            dataBuffer[dataIndex] = caseData;
            dataIndex++;
        }

        DrawArraysIndirectCommand command = marchingCubes.getCommandTemplate(i);
        command.instanceCount = m_solidCaseData[i].size();
        command.baseInstance = baseIndex;
        baseIndex += m_solidCaseData[i].size();
        chunk.addDrawCommand(command);
    }

    // Жидкие воксели
    baseIndex = 0;
    dataIndex = 0;
    for (int i = 1; i < 255; i++) {
        if (m_liquidCaseData[i].size() == 0) continue;

        for (GLuint caseData : m_liquidCaseData[i]) {
            dataBuffer[dataIndex] |= (caseData << 15);
            dataIndex++;
        }

        DrawArraysIndirectCommand command = marchingCubes.getCommandTemplate(i);
        command.instanceCount = m_liquidCaseData[i].size();
        command.baseInstance = baseIndex;
        baseIndex += m_liquidCaseData[i].size();
        chunk.addLiquidsDrawCommand(command);
    }

    if (chunk.getDrawCommandsCount() != 0) {
        m_checker.updateVisibilityStates(grid, chunk);
    }
    else {
        chunk.clearVisibilityStatesForEmptyChunk();
    }

    if (globalChunkSSBO.isInited()) {
        constexpr GLsizei dataPerChunk = VoxelChunk::MARCHING_CUBES_BYTE_SIZE;
        GLsizei dataOffset = dataPerChunk * chunk.getIdInSSBO();
        globalChunkSSBO.pushData(&dataBuffer[0], dataBuffer.size(), dataOffset);

        constexpr GLsizei gridSizePerChunk = VoxelChunk::GRID_BYTE_SIZE;
        GLsizei gridOffset = gridSizePerChunk * chunk.getIdInSSBO();
        auto& storage = chunk.getVoxelArray();
        globalChunkGridsSSBO.pushData(storage.getDataPtr(), storage.getSize(), gridOffset);
    }
    else {
        chunk.pushDataInSSBO(dataBuffer);
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
        std::cout << "ChunkBuilder: " << error << std::endl;
    }

    clear();
}

void engine::ChunkBuilder::accumulateCases(MarchingCubes &marchingCubes, ChunkGrid &grid, VoxelChunk &chunk) {
    constexpr int MARCHING_CUBES_COUNT = 32;

    // Определение количества отрисовок для каждой из фигур марширующих кубов и
    // упаковка данных фигур для последующей загрузки в буфер SSBO
    for (int z = 0; z < MARCHING_CUBES_COUNT; z++) {
        for (int y = 0; y < MARCHING_CUBES_COUNT; y++) {
            for (int x = 0; x < MARCHING_CUBES_COUNT; x++) {
                std::array<Voxel, 8> voxels {
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
}

void engine::ChunkBuilder::clear() {
    m_solidCubesCount = 0;
    m_liquidCubesCount = 0;

    for (int i = 0; i < CASE_COUNT; i++) {
        m_solidCaseData[i].clear();
        m_liquidCaseData[i].clear();
    }
}

void engine::ChunkBuilder::addMarchingCube(MarchingCubes& marchingCubes, std::array<Voxel, 8> &voxels, int x, int y, int z) {
    uint8_t caseId = getSolidCaseIndex(voxels);
    if (caseId != 0 || caseId != 255) {
        GLuint caseData = packData(x, y, z);
        m_solidCaseData[caseId].push_back(caseData);
        m_solidCubesCount++;
    }

    uint8_t liquidCaseId = getLiquidCaseIndex(voxels);
    if (liquidCaseId != 0 || liquidCaseId != 255) {
        GLuint caseData = packData(x, y, z);
        m_liquidCaseData[liquidCaseId].push_back(caseData);
        m_liquidCubesCount++;
    }
}

uint8_t engine::ChunkBuilder::getSolidCaseIndex(std::array<Voxel, 8>& voxels) {
    uint8_t caseId = voxels[0].isHaveSolid();
    caseId |= voxels[1].isHaveSolid() << 1;
    caseId |= voxels[2].isHaveSolid() << 2;
    caseId |= voxels[3].isHaveSolid() << 3;
    caseId |= voxels[4].isHaveSolid() << 4;
    caseId |= voxels[5].isHaveSolid() << 5;
    caseId |= voxels[6].isHaveSolid() << 6;
    caseId |= voxels[7].isHaveSolid() << 7;
    return caseId;
}

uint8_t engine::ChunkBuilder::getLiquidCaseIndex(std::array<Voxel,8>& voxels) {
    bool cube[8] = { 
        voxels[0].isHaveWater() , voxels[1].isHaveWater(), voxels[2].isHaveWater(), voxels[3].isHaveWater(), 
        voxels[4].isHaveWater() , voxels[5].isHaveWater(), voxels[6].isHaveWater(), voxels[7].isHaveWater()
    };
    
    if (voxels[0].isHaveSolid()) cube[0] |= voxels[1].isHaveWater() || voxels[3].isHaveWater();
    if (voxels[1].isHaveSolid()) cube[1] |= voxels[0].isHaveWater() || voxels[2].isHaveWater();
    if (voxels[2].isHaveSolid()) cube[2] |= voxels[1].isHaveWater() || voxels[3].isHaveWater();
    if (voxels[3].isHaveSolid()) cube[3] |= voxels[0].isHaveWater() || voxels[2].isHaveWater();

    if (voxels[4].isHaveSolid()) cube[4] |= voxels[5].isHaveWater() || voxels[7].isHaveWater();
    if (voxels[5].isHaveSolid()) cube[5] |= voxels[4].isHaveWater() || voxels[6].isHaveWater();
    if (voxels[6].isHaveSolid()) cube[6] |= voxels[5].isHaveWater() || voxels[7].isHaveWater();
    if (voxels[7].isHaveSolid()) cube[7] |= voxels[4].isHaveWater() || voxels[6].isHaveWater();

    uint8_t caseId = cube[0];
    caseId |= cube[1] << 1;
    caseId |= cube[2] << 2;
    caseId |= cube[3] << 3;
    caseId |= cube[4] << 4;
    caseId |= cube[5] << 5;
    caseId |= cube[6] << 6;
    caseId |= cube[7] << 7;
    return caseId;
}

GLuint engine::ChunkBuilder::packData(int x, int y, int z) {
    int left = x;
    left <<= 5;
    left |= y;
    left <<= 5;
    left |= z;

    return left;
}
