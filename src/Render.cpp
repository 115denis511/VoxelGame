#include "Render.h"

engine::Shader*         engine::Render::g_shaderFinal;
engine::Shader*         engine::Render::g_shaderMix_RGB_A;
engine::GBuffer*        engine::Render::g_gBuffer;
engine::Mesh*           engine::Render::g_primitivePlane;
engine::Mesh*           engine::Render::g_primitiveScreenPlane;

bool engine::Render::init() {
    glm::ivec2 viewport = WindowGLFW::getViewport();
    glViewport(0, 0, viewport.x, viewport.y);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    g_shaderFinal = new Shader("Shader/test3d.vert", "Shader/simple.frag");
    g_shaderMix_RGB_A = new Shader("Shader/fillViewport.vert", "Shader/mix_RGB_A.frag");

    g_gBuffer = new GBuffer(viewport);

    g_primitivePlane = buildPrimitivePlane(-0.5f, 0.5f, 0.5f, -0.5f);
    g_primitiveScreenPlane = buildPrimitivePlane(-1.f, 1.f, 1.f, -1.f);

    UniformManager::init();
    TextureManager::init(g_shaderMix_RGB_A, g_primitiveScreenPlane);

    // test
    glm::mat4 perspective = glm::perspective(glm::radians(45.f), (float)viewport.x / (float)viewport.y, 0.1f, 1000.f);
    Camera camera;
    camera.updateLookAt();
    uniform_structs::DrawVars vars(perspective * camera.getLookAt());
    UniformManager::setDrawVars(vars);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(5.f, 0.f, 0.f));
    model = glm::rotate(model, glm::radians(45.f), glm::vec3(0.f, -1.f, 0.f));
    model = glm::scale(model, glm::vec3(1.f));

    TextureArrayRef texture = TextureManager::addMixedTexture_RGB_A("container.jpg", "container.jpg");
    TextureManager::addTexture("rock.png");
    TextureManager::updateMipmapsAndMakeResident();
    g_shaderFinal->use();
    g_shaderFinal->setBindlessSampler("bindless", texture.getHandler());
    g_shaderFinal->setMat4("model", model);

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
        std::cout << "engine::Render::init() " + error << std::endl;
    }

    //test end

    if (Log::isHaveFatalError()) return false;
    return true;
}

void engine::Render::freeResources() {
    delete g_shaderFinal;

    delete g_gBuffer;

    delete g_primitivePlane;
    delete g_primitiveScreenPlane;

    UniformManager::freeResources();
    TextureManager::freeResources();
}

void engine::Render::draw() {
    if (WindowGLFW::g_isRenderMustUpdateViewport) {
        updateViewports();
    }
    //g_resources->m_gBuffer->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    g_primitivePlane->draw();

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
        std::cout << "DrawLoop " << error << std::endl;
    }
}

void engine::Render::updateViewports() {
    glm::ivec2 viewport = WindowGLFW::getViewport();
    
    glViewport(0, 0, viewport.x, viewport.y);

    g_gBuffer->updateViewport(viewport);

    WindowGLFW::g_isRenderMustUpdateViewport = false;
}

engine::Mesh *engine::Render::buildPrimitivePlane(GLfloat leftX, GLfloat topY, GLfloat rightX, GLfloat bottomY) {
    Vertex vertices[] = {
        Vertex{glm::vec3(leftX,  bottomY, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(rightX, bottomY, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(rightX, topY,    0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)},
        Vertex{glm::vec3(leftX,  topY,    0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)}
    };

    constexpr GLuint indeces[] = {
        0, 1, 2,   // Первый треугольник
        0, 2, 3,   // Второй треугольник
    };

    return new Mesh(vertices, std::size(vertices), indeces, std::size(indeces));
}
