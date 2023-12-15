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
    while(!WindowGLFW::isShouldClose()) {
        WindowGLFW::poolEvents();

        Render::draw();

        WindowGLFW::swapBuffers();
    }
}

void engine::Core::close() {
    Render::onClose();

    Scene::onClose();

    WindowGLFW::terminate();
}

void engine::Core::onFatalError() {
    Log::save();
}
