#include "engine.h"

bool engine::Core::init() {
    Controls::init();

    constexpr int constWidth = 1300, constHeight = 720; // волшебные числа
    if (!WindowGLFW::init(constWidth, constHeight)) {
        Log::addFatalError("engine::Core::init: ERROR! Failed to initialize GLFW");
        onFatalError();
		return false;
    }

    glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		Log::addFatalError("engine::Core::init: ERROR! Failed to initialize GLEW");
        onFatalError();
		return false;
	}
    if (!GLEW_ARB_bindless_texture) {
        Log::addFatalError("engine::Core::init: ERROR! GPU don't support bindless textures");
        onFatalError();
		return false;
    }

    if (!Render::init()) {
		Log::addFatalError("engine::Core::init: ERROR! Failed to initialize render resources");
        onFatalError();
		return false;
	}

    if (!Scene::init()) {
		Log::addFatalError("engine::Core::init: ERROR! Failed to initialize scene resources");
        onFatalError();
		return false;
	}

    return true;
}

void engine::Core::start(ISceneLogic* sceneLogic) {
    if (sceneLogic == nullptr) {
        Log::addFatalError("ERROR! engine::Core::start() sceneLogic is NULL");
    }
    Scene::setCurrentSceneLogic(sceneLogic);

    // TEST
    Transform trs;
    trs.setRotation(glm::vec3(-45.f, 0.f, 0.f));
    RenderComponent rc(AssetManager::getModel("Model/simpleChar.gltf"));

    for (size_t i = 0; i < 50; i++) {
        for (size_t j = 0; j < 50; j++){
            trs.setPosition(glm::vec3(5.f + i, 2.f, (float)j * 2 ));
            Scene::g_requests->addEntity(trs, rc);
        }
    }
    Scene::g_requests->deleteEntity(1);
    Scene::g_requests->deleteEntity(3);
    Scene::g_requests->deleteEntity(5);

    MarchingCubesManager::init();
    // REMEMBER DELETE FREE RESOURCES AT MAIN LOOP END
    Shader mcShader("Shader/marchingCubes.vert", "Shader/marchingCubes.frag"); 
    mcShader.use();
    mcShader.setInt("tex", 0);
    auto marching = MarchingCubesManager::getInstance();
    marching->resizeChunkGrid(24);

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
        std::cout << "CORE TEST INIT " << error << std::endl;
    }
    // TEST END



    while(!WindowGLFW::isShouldClose() && !Log::isHaveFatalError()) {
        GLfloat currentFrame = (float)glfwGetTime();
        static int frameCount = 0;
        static float prevTime = 0;
        frameCount++;
		if (currentFrame - prevTime >= 1.0f) {
            WindowGLFW::setTitle(std::string("fps: ") + std::to_string(frameCount).c_str());
			frameCount = 0;
			prevTime = currentFrame;
		}

        WindowGLFW::poolEvents();

        Render::draw(
            Scene::g_camera.getVars(),
            *Scene::getSceneResources(),
            Scene::g_worldBVH);

        mcShader.use();
        marching->draw(mcShader);
        marching->updateChunks();

        //glm::vec3 cameraPos = Scene::g_camera.getPosition();
        //float cellSize = 32.f;
        //std::cout << "X: " << std::floor(cameraPos.x / cellSize) << " | Y: " << std::floor(cameraPos.y / cellSize) << " | Z: " << std::floor(cameraPos.z / cellSize) << "\n";

        Scene::sceneLogicUpdatePhase();

        Scene::applyChangesPhase();

        Scene::applyRequestsPhase();

        Controls::resetState();

        WindowGLFW::swapBuffers();
    }

    if (Log::isHaveFatalError()) {
        onFatalError();
    }

    // TEST !!!!!!
    marching = MarchingCubesManager::getInstance();
    MarchingCubesManager::freeResources();
}

void engine::Core::close() {
    Render::freeResources();

    Scene::freeResources();

    WindowGLFW::terminate();
}

void engine::Core::onFatalError() {
    Log::save();
}
