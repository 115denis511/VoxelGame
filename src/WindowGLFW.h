#ifndef __WINDOW_GLFW_H__
#define __WINDOW_GLFW_H__

#include "stdafx.h"
#include "Controls.h"

namespace engine {
    namespace glfw_callbacks {
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
        void character_callback(GLFWwindow* window, unsigned int codepoint);
        void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    }
    
    class WindowGLFW {
    public:
        static bool init(int width, int height);
        static void terminate();
        static void poolEvents();
        static void swapBuffers();
        static bool isShouldClose();
        static void hideMouseCursor();
        static void showMouseCursor();
        static glm::ivec2 getViewport();

    private:
        WindowGLFW() {}

        static GLFWwindow*  g_window;
        static glm::ivec2   g_viewport;
    };
}

#endif