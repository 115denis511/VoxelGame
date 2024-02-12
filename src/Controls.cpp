#include "Controls.h"

bool                engine::Controls::g_keys[1024];
std::vector<int>    engine::Controls::g_keysDown;
std::vector<int>    engine::Controls::g_keysUp;
glm::vec2           engine::Controls::g_mousePos;
glm::vec2           engine::Controls::g_mouseOffset;

void engine::Controls::init() {
    for (int i = 0; i < 1024; i++) {
        g_keys[i] = false;
    }
}

void engine::Controls::resetState() {
    g_mouseOffset.x = 0;
    g_mouseOffset.y = 0;

    g_keysDown.clear();
    g_keysUp.clear();
}

bool engine::Controls::isKeyPressed(int key) {
    return g_keys[key];
}

const std::vector<int> &engine::Controls::getKeysDown() {
    return g_keysDown;
}

const std::vector<int> &engine::Controls::getKeysUp() {
    return g_keysUp;
}

const glm::vec2 engine::Controls::getMouseOffset() {
    return g_mouseOffset;
}

void engine::Controls::setMousePosition(GLfloat x, GLfloat y) {
    g_mouseOffset.x = 0;
    g_mouseOffset.y = 0;

    g_mousePos.x = x;
    g_mousePos.y = y;

}