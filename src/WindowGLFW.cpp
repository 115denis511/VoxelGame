#include "WindowGLFW.h"

GLFWwindow*  engine::WindowGLFW::g_window = nullptr;
glm::ivec2   engine::WindowGLFW::g_viewport;

bool engine::WindowGLFW::init(int width, int height) {
    if (!glfwInit()) {
		return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    g_window = glfwCreateWindow(width, height, "VoxelGame", NULL, NULL);
    glfwGetFramebufferSize(g_window, &g_viewport.x, &g_viewport.y);

    glfwMakeContextCurrent(g_window);

    glfwSetKeyCallback(g_window, glfw_callbacks::key_callback);
    glfwSetCharCallback(g_window, glfw_callbacks::character_callback);
	glfwSetCursorPosCallback(g_window, glfw_callbacks::mouse_callback);
    glfwSetMouseButtonCallback(g_window, glfw_callbacks::mouse_button_callback);
    glfwSetScrollCallback(g_window, glfw_callbacks::scroll_callback);
    glfwSetFramebufferSizeCallback(g_window, glfw_callbacks::framebuffer_size_callback);

    showMouseCursor();

    return true;
}

void engine::WindowGLFW::terminate() {
    glfwTerminate();
}

void engine::WindowGLFW::poolEvents() {
    glfwPollEvents();
}

bool engine::WindowGLFW::isShouldClose() {
    return glfwWindowShouldClose(g_window);
}

void engine::WindowGLFW::swapBuffers() {
    glfwSwapBuffers(g_window);
}

void engine::WindowGLFW::hideMouseCursor()
{
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(g_window, (GLfloat)(g_viewport.x / 2), (GLfloat)(g_viewport.y / 2));
    Controls::setMousePosition((GLfloat)(g_viewport.x / 2), (GLfloat)(g_viewport.y / 2));
}

void engine::WindowGLFW::showMouseCursor() {
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPos(g_window, (GLfloat)(g_viewport.x / 2), (GLfloat)(g_viewport.y / 2));
    Controls::setMousePosition((GLfloat)(g_viewport.x / 2), (GLfloat)(g_viewport.y / 2));
}

glm::ivec2 engine::WindowGLFW::getViewport() {
    return g_viewport;
}
