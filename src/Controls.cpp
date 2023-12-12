#include "Controls.h"

bool         engine::Controls::g_keys[1024];
glm::vec2    engine::Controls::g_mousePos;
glm::vec2    engine::Controls::g_mouseOffset;

void engine::Controls::init() {
    for (int i = 0; i < 1024; i++) {
        g_keys[i] = false;
    }
}

void engine::Controls::setMousePosition(GLfloat x, GLfloat y) {
    g_mousePos.x = x;
    g_mousePos.y = y;

    g_mouseOffset.x = x;
    g_mouseOffset.y = y;
}