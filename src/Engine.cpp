#include "engine.h"

bool engine::Core::init() {
    Controls::init();

    constexpr int constWidth = 1300, constHeight = 720; // волшебные числа
    if (!WindowGLFW::init(constWidth, constHeight)) {
        Log::addMessage("engine::Core::init: Failed to initialize GLFW");
		return false;
    }

    glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		Log::addMessage("engine::Core::init: Failed to initialize GLEW");
		return false;
	}

    if (!Scene::init()) {
		Log::addMessage("engine::Core::init: Failed to initialize scene resources");
		return false;
	}

    return true;
}

void engine::Core::start() {
    while(!WindowGLFW::isShouldClose()) {
        WindowGLFW::poolEvents();

        WindowGLFW::swapBuffers();
    }
}

void engine::Core::close() {
    Scene::onClose();

    WindowGLFW::terminate();
}
