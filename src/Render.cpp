#include "Render.h"

engine::Shader*                 engine::Render::g_shaderFinal;
engine::Shader*                 engine::Render::g_shaderMix_RGB_A;
engine::Shader*                 engine::Render::g_shaderFill_RGB;
engine::GBuffer*                engine::Render::g_gBuffer;
engine::MeshRef                 engine::Render::g_primitiveFullScreenRect;
engine::ProjectionPerspective   engine::Render::g_projectionPerspective;
std::vector<engine::Model*>     engine::Render::g_modelsToInstancedDraw;

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

    g_projectionPerspective = ProjectionPerspective(viewport, 
                                                    45.f,
                                                    0.1f, 
                                                    1000.f);

    AssetManager::init(g_shaderMix_RGB_A, g_shaderFill_RGB);
    ShaderStorageManager::init();

    // test
    std::string modelPath = "Model/simpleChar.gltf";
    test_model = AssetManager::addModel(modelPath);

    Camera camera;
    camera.updateLookAt();
    uniform_structs::DrawVars vars(g_projectionPerspective.getMatrix() * camera.getLookAt());
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

    /*unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";*/

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
    ShaderStorageManager::freeResources();
}

void engine::Render::draw(CameraVars cameraVars, SceneResources& sceneResources, BVHTree& worldBVH) {
    if (WindowGLFW::g_isRenderMustUpdateViewport) {
        updateViewports();
    }

    uniform_structs::DrawVars vars(g_projectionPerspective.getMatrix() * cameraVars.lookAt, g_projectionPerspective.getMatrix(), cameraVars.lookAt);
    UniformManager::setDrawVars(vars);

    //g_resources->m_gBuffer->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //test_model->draw();
    //g_primitiveFullScreenRect.draw();
    //MeshManager::getPrimitiveCube(1.f)->draw();
    //MeshManager::getPrimitiveSphere(1.f, 32, 32)->draw();

    accamulateInstancingBuffers(sceneResources, worldBVH, Frustum(cameraVars, g_projectionPerspective));
    drawInstanced();
     

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

    g_projectionPerspective.update(viewport);

    WindowGLFW::g_isRenderMustUpdateViewport = false;
}

void engine::Render::accamulateInstancingBuffers(SceneResources& sceneResources, BVHTree& worldBVH, Frustum frustum) {
    std::vector<int> overlaps = worldBVH.getOverlapsedRenderComponents(frustum);
    
    for (size_t i = 0; i < overlaps.size(); i++) {
        int componentId = overlaps[i];
        auto renderComponent = sceneResources.renderComponents.get(componentId);

        auto model = renderComponent.getObject().getModel();

        if (model->getInstancingData().getCount() == 0) {
            addToInstancedDrawList(model);
        }
        renderComponent.getObject().pushTransformIdToInstancingBuffer();
    }
}

void engine::Render::drawInstanced() {
    g_shaderFinal->use();
    for (size_t i = 0; i < g_modelsToInstancedDraw.size(); i++) {
        g_modelsToInstancedDraw[i]->drawInstanced();
    }
    g_modelsToInstancedDraw.clear();
}

void engine::Render::addToInstancedDrawList(Model* model) {
    g_modelsToInstancedDraw.push_back(model);
}
