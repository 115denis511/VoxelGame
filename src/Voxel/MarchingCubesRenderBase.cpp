#include "MarchingCubesRenderBase.h"

engine::MarchingCubesRenderBase::~MarchingCubesRenderBase() {
    glDeleteBuffers(1, &m_commandBuffer);

    if (m_shaderSolid != nullptr) delete m_shaderSolid;
}

engine::MarchingCubesRenderBase::MarchingCubesRenderBase(
    GLuint maxChunksPerDraw,
    const GLchar* shaderSolidVertPath, 
    const GLchar* shaderSolidFragPath
) {
    m_drawCommands.resize(254 * maxChunksPerDraw);
    m_drawBufferRefs.resize(254 * maxChunksPerDraw);

    glCreateBuffers(1, &m_commandBuffer);
    GLuint byteSize = sizeof(engine::DrawArraysIndirectCommand) * 254 * maxChunksPerDraw;
    glNamedBufferData(m_commandBuffer, byteSize, NULL, GL_DYNAMIC_DRAW);

    m_shaderSolid = new Shader(shaderSolidVertPath, shaderSolidFragPath);
}

bool engine::MarchingCubesRenderBase::setVoxelTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents) {
    return m_textures.setTexture(layer, rawImage, width, height, nrComponents);
}

bool engine::MarchingCubesRenderBase::setVoxelTexture(int layer, std::string path) {
    return m_textures.setTexture(layer, path);
}

bool engine::MarchingCubesRenderBase::setVoxelTexture(int layer, glm::vec4 color) {
    return m_textures.setTexture(layer, color);
}

void engine::MarchingCubesRenderBase::startTextureEditing() {
    if (m_textures.isResident()) m_textures.makeNonResident();
}

void engine::MarchingCubesRenderBase::endTextureEditing() {
    if (!m_textures.isResident()) {
        m_textures.updateMipmap();
        m_textures.makeResident();
    }
}
