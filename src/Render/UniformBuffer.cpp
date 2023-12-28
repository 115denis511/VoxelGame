#include "UniformBuffer.h"

engine::UniformBuffer::UniformBuffer(GLuint blockIndex, GLsizeiptr byteSize) {
    m_byteSize = byteSize;

    glCreateBuffers(1, &m_UBO);
    glNamedBufferData(m_UBO, byteSize, NULL, GL_DYNAMIC_DRAW);

    m_blockIndex = blockIndex;
    glBindBufferRange(GL_UNIFORM_BUFFER, m_blockIndex, m_UBO, 0, m_byteSize);

}

engine::UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &m_UBO);
}

const GLuint engine::UniformBuffer::getUBO() {
    return m_UBO;
}

const GLuint engine::UniformBuffer::getBlockIndex() {
    return m_blockIndex;
}
