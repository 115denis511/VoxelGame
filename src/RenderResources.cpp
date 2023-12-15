#include "RenderResources.h"

engine::RenderResources* engine::RenderResources::g_selfRef;
bool engine::RenderResources::g_isMustUpdateViewports; 

void engine::RenderResources::updateViewports() {
    g_isMustUpdateViewports = true;
}

void engine::RenderResources::init(glm::ivec2 viewport) {
    g_isMustUpdateViewports = false;

    m_shaderFinal = new Shader("Shader/simple.vert", "Shader/simple.frag");

    m_gBuffer = new GBuffer(viewport);

    m_primitivePlane = ModelBuilder::buildPrimitivePlane();
    m_primitiveScreenPlane = ModelBuilder::buildPrimitiveScreenPlane();
}

void engine::RenderResources::onClose() {
    delete m_shaderFinal;

    delete m_gBuffer;

    delete m_primitivePlane;
    delete m_primitiveScreenPlane;
}
