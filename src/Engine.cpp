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

void engine::Core::start() {
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

        Render::draw();

        WindowGLFW::swapBuffers();
    }
}

void engine::Core::close() {
    Render::freeResources();

    Scene::freeResources();

    WindowGLFW::terminate();
}

void engine::Core::onFatalError() {
    Log::save();
}
