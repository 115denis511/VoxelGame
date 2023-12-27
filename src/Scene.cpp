#include "Scene.h"

engine::SceneResources* engine::Scene::g_resouces = nullptr;

bool engine::Scene::init() {
    g_resouces = new SceneResources();

    return true;
}

void engine::Scene::freeResources() {
    delete g_resouces;
}
