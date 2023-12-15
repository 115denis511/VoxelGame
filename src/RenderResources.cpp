#include "RenderResources.h"

engine::RenderResources::RenderResources(glm::ivec2 viewport) {
    m_shaderFinal = new Shader("Shader/simple.vert", "Shader/simple.frag");

    m_gBuffer = new GBuffer(viewport);

    m_primitivePlane = ModelBuilder::buildPrimitivePlane();
    m_primitiveScreenPlane = ModelBuilder::buildPrimitiveScreenPlane();
}

engine::RenderResources::~RenderResources() {
    delete m_shaderFinal;

    delete m_gBuffer;

    delete m_primitivePlane;
    delete m_primitiveScreenPlane;
}
