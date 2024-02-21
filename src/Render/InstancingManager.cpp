#include "InstancingManager.h"

GLuint engine::InstancingManager::g_instancingMatricesSSBO;

bool engine::InstancingManager::init() {
    glCreateBuffers(1, &g_instancingMatricesSSBO);
    constexpr GLuint byteSize = sizeof(glm::mat4) * engine_properties::SCENE_MAX_RENDER_COMPONENTS;
    glNamedBufferData(g_instancingMatricesSSBO, byteSize, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, engine_properties::SSBO_INSTANCING_MATRICES_BLOCK_ID, g_instancingMatricesSSBO);

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
        std::cout << "InstancingManager " << error << std::endl;
    }
    if (errors != 0) {
        return false;
    }

    return true;
}

void engine::InstancingManager::freeResources() {
    glDeleteBuffers(1, &g_instancingMatricesSSBO);
}

void engine::InstancingManager::pushMatrices(const glm::mat4* buffer, 	GLsizei count) {
    glNamedBufferSubData(g_instancingMatricesSSBO, 0, sizeof(glm::mat4) * count, buffer);
}
