#include "engine.h"

GLFWwindow* engine::Core::g_window = nullptr;

bool engine::Core::init() {
    if (!glfwInit())
    {
        Log::addMessage("engine::Core::init: Failed to initialize GLFW");
		return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // TODO: Размер создаваемого окна должен задаваться из сохранённых настроек
    constexpr int constWidth = 1300, constHeight = 720; // волшебные числа
    g_window = glfwCreateWindow(constWidth, constHeight, "My Title", NULL, NULL);

    glfwMakeContextCurrent(g_window);

    glfwSetKeyCallback(g_window, glfw_callbacks::key_callback);
    glfwSetCharCallback(g_window, glfw_callbacks::character_callback);
	glfwSetCursorPosCallback(g_window, glfw_callbacks::mouse_callback);
    glfwSetMouseButtonCallback(g_window, glfw_callbacks::mouse_button_callback);
    glfwSetScrollCallback(g_window, glfw_callbacks::scroll_callback);
    glfwSetFramebufferSizeCallback(g_window, glfw_callbacks::framebuffer_size_callback);


    glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		Log::addMessage("engine::Core::init: Failed to initialize GLEW");
		return false;
	}

    return true;
}

void engine::Core::start() {
    while(!glfwWindowShouldClose(g_window)) {
        glfwPollEvents();

        glfwSwapBuffers(g_window);
    }
}

void engine::Core::close() {
    glfwTerminate();
}
