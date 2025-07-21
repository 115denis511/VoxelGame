#include "ShaderStorageManager.h"

GLuint engine::ShaderStorageManager::g_instancingMatricesSSBO;
GLuint engine::ShaderStorageManager::g_transformMatricesSSBO;
GLuint engine::ShaderStorageManager::g_chunkBufferRefSSBO;

bool engine::ShaderStorageManager::init() {
    glCreateBuffers(1, &g_instancingMatricesSSBO);
    constexpr GLuint byteSizeInstancing = sizeof(int) * engine_properties::SCENE_MAX_RENDER_COMPONENTS;
    glNamedBufferData(g_instancingMatricesSSBO, byteSizeInstancing, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, engine_properties::SSBO_INSTANCING_MATRICES_BLOCK_ID, g_instancingMatricesSSBO);

    glCreateBuffers(1, &g_transformMatricesSSBO);
    constexpr GLuint byteSizeTransforms = sizeof(glm::mat4) * engine_properties::SCENE_MAX_RENDER_COMPONENTS;
    glNamedBufferData(g_transformMatricesSSBO, byteSizeTransforms, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, engine_properties::SSBO_TRANSFORM_MATRICES_BLOCK_ID, g_transformMatricesSSBO);

    glCreateBuffers(1, &g_chunkBufferRefSSBO);
    constexpr GLuint byteSizeChunkRefs = sizeof(GLuint) * 254 * 14;
    glNamedBufferData(g_chunkBufferRefSSBO, byteSizeChunkRefs, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, engine_properties::SSBO_CHUNK_BUFFER_REF_ID, g_chunkBufferRefSSBO);

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
        std::cout << "ShaderStorageManager " << error << std::endl;
    }
    if (errors != 0) {
        return false;
    }

    return true;
}

void engine::ShaderStorageManager::freeResources() {
    glDeleteBuffers(1, &g_instancingMatricesSSBO);
    glDeleteBuffers(1, &g_transformMatricesSSBO);
    glDeleteBuffers(1, &g_chunkBufferRefSSBO);
}

void engine::ShaderStorageManager::pushInstancingTransformIds(const int* buffer, GLsizei count) {
    glNamedBufferSubData(g_instancingMatricesSSBO, 0, sizeof(int) * count, buffer);
}

glm::mat4 *engine::ShaderStorageManager::getMappedTransformsSSBO() {
    constexpr GLuint size = sizeof(glm::mat4) * engine_properties::SCENE_MAX_RENDER_COMPONENTS;
    return reinterpret_cast<glm::mat4*>(glMapNamedBufferRange(g_transformMatricesSSBO, 0, size, GL_MAP_WRITE_BIT));
}

void engine::ShaderStorageManager::unmapTransformsSSBO() {
    glUnmapNamedBuffer(g_transformMatricesSSBO);
}

void engine::ShaderStorageManager::setChunkBufferRef(const std::array<GLuint, 254 * 14> &refs, GLsizei count) {
    glNamedBufferSubData(g_chunkBufferRefSSBO, 0, sizeof(GLuint) * count, &refs);
}

void engine::ShaderStorageManager::resetBindings() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, engine_properties::SSBO_INSTANCING_MATRICES_BLOCK_ID, g_instancingMatricesSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, engine_properties::SSBO_TRANSFORM_MATRICES_BLOCK_ID, g_transformMatricesSSBO);
}
