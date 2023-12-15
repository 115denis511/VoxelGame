#include "Render.h"

engine::RenderResources engine::Render::g_resources;

bool engine::Render::init() {
    glm::ivec2 viewport = WindowGLFW::getViewport();
    glViewport(0, 0, viewport.x, viewport.y);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    g_resources.init(viewport);
    g_resources.g_selfRef = &g_resources;

    if (Log::isHaveFatalError()) return false;
    return true;
}

void engine::Render::onClose() {
    g_resources.onClose();
}

void engine::Render::draw() {
    if (g_resources.g_isMustUpdateViewports) {
        updateViewports();
        g_resources.g_isMustUpdateViewports = false;
    }
    //g_resources->m_gBuffer->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    g_resources.m_shaderFinal->use();
    g_resources.m_primitivePlane->draw();

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
        std::cout << error << std::endl;
    }
}

void engine::Render::updateViewports() {
    glm::ivec2 viewport = WindowGLFW::getViewport();
    
    glViewport(0, 0, viewport.x, viewport.y);

    g_resources.m_gBuffer->updateViewport(viewport);
}
