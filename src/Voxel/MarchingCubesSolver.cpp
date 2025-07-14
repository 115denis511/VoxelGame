#include "MarchingCubesSolver.h"

engine::MarchingCubesSolver::MarchingCubesSolver() {

}

void engine::MarchingCubesSolver::regenerateChunk(MarchingCubes &marchingCubes, VoxelChunk &chunk) {
    constexpr short MARCHING_CUBES_COUNT = 31;
    int cubesCount = 0;

    // Определение количества отрисовок для каждой из фигур марширующих кубов и
    // упаковка данных фигур для последующей загрузки в буфер SSBO
    for (short z = 0; z < MARCHING_CUBES_COUNT; z++) {
        for (short y = 0; y < MARCHING_CUBES_COUNT; y++) {
            for (short x = 0; x < MARCHING_CUBES_COUNT; x++) {
                MarchingCubesVoxel voxels[8] {
                    chunk.getVoxel(x, y + 1, z + 1), // topFrontLeft
                    chunk.getVoxel(x + 1, y + 1, z + 1), // topFrontRight
                    chunk.getVoxel(x + 1, y + 1, z), // topBackRight
                    chunk.getVoxel(x, y + 1, z), // topBackLeft
                    chunk.getVoxel(x, y, z + 1), // bottomFrontLeft
                    chunk.getVoxel(x + 1, y, z + 1), // bottomFrontRight
                    chunk.getVoxel(x + 1, y, z), // bottomBackRight
                    chunk.getVoxel(x, y, z) // bottomBackLeft
                };

                uint8_t caseId = getCaseIndex(voxels);
                if (caseId == 0 || caseId == 255) continue;

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
                cubesCount++;
            }
        }
    }

    // Сборка коммандного буфера
    std::vector<DrawArraysIndirectCommand> commandBuffer;
    std::vector<glm::ivec2> dataBuffer;
    dataBuffer.reserve(cubesCount);
    int baseIndex = 0;
    for (int i = 1; i < 255; i++) {
        if (m_caseData[i].size() == 0) continue;
        dataBuffer.insert(dataBuffer.end(), m_caseData[i].begin(), m_caseData[i].end());
        DrawArraysIndirectCommand command = marchingCubes.getCommandTemplate(i);
        command.instanceCount = m_caseData[i].size();
        command.baseInstance = baseIndex;
        baseIndex += m_caseData[i].size();
        commandBuffer.push_back(command);
    }

    if (commandBuffer.size() != 0) {
        chunk.updateVisibilityStates();
    }

    GLuint commandBufferObject = chunk.getCommandBuffer(), ssboBuffer = chunk.getSSBO();
    chunk.setDrawCount(commandBuffer.size());
    int commandBufferSize = commandBuffer.size() * sizeof(DrawArraysIndirectCommand);
    glNamedBufferSubData(commandBufferObject, 0, commandBufferSize, &commandBuffer[0]);
    int ssboSize = dataBuffer.size() * sizeof(glm::ivec2);
    glNamedBufferSubData(ssboBuffer, 0, ssboSize, &dataBuffer[0]);

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

void engine::MarchingCubesSolver::clear() {
    for (std::vector<glm::ivec2>& caseBuffer : m_caseData) {
        caseBuffer.clear();
    }
}

uint8_t engine::MarchingCubesSolver::getCaseIndex(MarchingCubesVoxel voxels[8]) {
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
