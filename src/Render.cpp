#include "Render.h"

engine::Shader*         engine::Render::g_shaderFinal;
engine::Shader*         engine::Render::g_shaderMix_RGB_A;
engine::Shader*         engine::Render::g_shaderFill_RGB;
engine::GBuffer*        engine::Render::g_gBuffer;
engine::MeshRef         engine::Render::g_primitiveFullScreenRect;
glm::mat4               engine::Render::g_perspectiveProjection;
float                   engine::Render::g_perspectiveProjectionFov;
float                   engine::Render::g_perspectiveProjectionNear;
float                   engine::Render::g_perspectiveProjectionFar;

engine::Model*          engine::Render::test_model;

bool engine::Render::init() {
    glm::ivec2 viewport = WindowGLFW::getViewport();
    glViewport(0, 0, viewport.x, viewport.y);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    g_shaderFinal = new Shader("Shader/test3d.vert", "Shader/simple.frag");
    g_shaderMix_RGB_A = new Shader("Shader/fillViewport_InvertY.vert", "Shader/mix_RGB_A.frag");
    g_shaderFill_RGB = new Shader("Shader/fillViewport_InvertY.vert", "Shader/fill_RGB.frag");

    g_gBuffer = new GBuffer(viewport);

    g_primitiveFullScreenRect = MeshManager::getPrimitiveRect(-1.f, 1.f, 1.f, -1.f)->getMeshRef();

    g_perspectiveProjectionFov = 45.f;
    g_perspectiveProjectionNear = 0.1f;
    g_perspectiveProjectionFar = 1000.f;
    g_perspectiveProjection = glm::perspective(
        glm::radians(g_perspectiveProjectionFov), 
        (float)viewport.x / (float)viewport.y, 
        g_perspectiveProjectionNear, 
        g_perspectiveProjectionFar);

    AssetManager::init(g_shaderMix_RGB_A, g_shaderFill_RGB);

    // test
    std::string modelPath = "Model/simpleChar.gltf";
    test_model = AssetManager::addModel(modelPath);

    Camera camera;
    camera.updateLookAt();
    uniform_structs::DrawVars vars(g_perspectiveProjection * camera.getLookAt());
    UniformManager::setDrawVars(vars);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(5.f, 0.f, 0.f));
    model = glm::rotate(model, glm::radians(45.f), glm::vec3(0.f, -1.f, 0.f));
    model = glm::scale(model, glm::vec3(1.f));

    TextureArrayRef texture = TextureManager::addMixedTexture_RGB_A("container.jpg", "container.jpg", glm::vec4(0.f));
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
    delete g_shaderMix_RGB_A;
    delete g_shaderFill_RGB;

    delete g_gBuffer;

    AssetManager::freeResources();
}

void engine::Render::draw(CameraVars cameraVars, SceneResources& sceneResources) {
    if (WindowGLFW::g_isRenderMustUpdateViewport) {
        updateViewports();
    }

    uniform_structs::DrawVars vars(g_perspectiveProjection * cameraVars.lookAt);
    UniformManager::setDrawVars(vars);

    //g_resources->m_gBuffer->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //test_model->draw();
    //g_primitiveFullScreenRect.draw();
    //MeshManager::getPrimitiveCube(1.f)->draw();
    //MeshManager::getPrimitiveSphere(1.f, 32, 32)->draw();

    for (int i = 0; i <= sceneResources.renderComponents.getBiggestUsedId(); i++) {
        auto renderComponent = sceneResources.renderComponents.get(i);

        if(renderComponent.isInUse()) {
            renderComponent.getObject().draw();
            g_shaderFinal->setMat4("model", renderComponent.getObject().getMatrix());
        }
    }
     

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

    g_perspectiveProjection = glm::perspective(
        glm::radians(g_perspectiveProjectionFov), 
        (float)viewport.x / (float)viewport.y, 
        g_perspectiveProjectionNear, 
        g_perspectiveProjectionFar);

    WindowGLFW::g_isRenderMustUpdateViewport = false;
}
