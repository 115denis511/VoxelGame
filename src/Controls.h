#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#include "stdafx.h"

namespace engine {
    class WindowGLFW;
    namespace glfw_callbacks {
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
        void character_callback(GLFWwindow* window, unsigned int codepoint);
        void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    }

    class Controls {
        friend WindowGLFW;
        friend void glfw_callbacks::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
        friend void glfw_callbacks::mouse_callback(GLFWwindow* window, double xpos, double ypos);
        friend void glfw_callbacks::character_callback(GLFWwindow* window, unsigned int codepoint);
        friend void glfw_callbacks::mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        friend void glfw_callbacks::scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        friend void glfw_callbacks::framebuffer_size_callback(GLFWwindow* window, int width, int height);

    public:
        static void init();
        static void resetState();
        static bool isKeyPressed(int key);
        static const std::vector<int>& getKeysDown();
        static const std::vector<int>& getKeysUp();
        static bool isMouseButtonPressed(int button);
        static bool isMouseButtonDown(int button);
        static bool isMouseButtonUp(int button);
        static const glm::vec2 getMouseOffset();

    private:
        Controls() {}

        static bool             g_keys[1024];
        static std::vector<int> g_keysDown;
        static std::vector<int> g_keysUp;
        static bool             g_mouseButtons[5];
        static bool             g_mouseButtonDown[5];
        static bool             g_mouseButtonUp[5];
        static glm::vec2        g_mousePos;
        static glm::vec2        g_mouseOffset;

        static void setMousePosition(GLfloat x, GLfloat y);
    };
}

#endif