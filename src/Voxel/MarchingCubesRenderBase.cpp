#include "MarchingCubesRenderBase.h"

engine::MarchingCubesRenderBase::~MarchingCubesRenderBase() {
    glDeleteBuffers(1, &m_commandBuffer);

    if (m_shaderSolids != nullptr) delete m_shaderSolids;
    if (m_shaderLiquids != nullptr) delete m_shaderLiquids;
}

engine::MarchingCubesRenderBase::MarchingCubesRenderBase(
    GLuint maxChunksPerDraw,
    const GLchar* shaderSolidsVertPath, 
    const GLchar* shaderSolidsFragPath,
    const GLchar* shaderLiquidsVertPath, 
    const GLchar* shaderLiquidsFragPath
) {
    m_solidsDrawCommands.resize(254 * maxChunksPerDraw);
    m_liquidsDrawCommands.resize(254 * maxChunksPerDraw);
    m_solidsDrawBufferRefs.resize(254 * maxChunksPerDraw);
    m_liquidsDrawBufferRefs.resize(254 * maxChunksPerDraw);

    glCreateBuffers(1, &m_commandBuffer);
    GLuint byteSize = sizeof(engine::DrawArraysIndirectCommand) * 254 * maxChunksPerDraw;
    glNamedBufferData(m_commandBuffer, byteSize, NULL, GL_DYNAMIC_DRAW);

    m_shaderSolids = new Shader(shaderSolidsVertPath, shaderSolidsFragPath);
    m_shaderLiquids = new Shader(shaderLiquidsVertPath, shaderLiquidsFragPath);
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
