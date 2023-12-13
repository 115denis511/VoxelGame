#include "WindowGLFW.h"

void engine::glfw_callbacks::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS)
		Controls::g_keys[key] = true;
	else if (action == GLFW_RELEASE)
		Controls::g_keys[key] = false;
}

void engine::glfw_callbacks::character_callback(GLFWwindow *window, unsigned int codepoint) {

}

void engine::glfw_callbacks::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    Controls::g_mouseOffset.x = (float)xpos - Controls::g_mousePos.x;
	Controls::g_mouseOffset.y = Controls::g_mousePos.y - (float)ypos;

    Controls::g_mousePos.x = (float)xpos;
    Controls::g_mousePos.y = (float)ypos;
}

void engine::glfw_callbacks::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {

}

void engine::glfw_callbacks::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {

}

void engine::glfw_callbacks::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glfwGetFramebufferSize(WindowGLFW::g_window, &WindowGLFW::g_viewport.x, &WindowGLFW::g_viewport.y);
}