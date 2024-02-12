#define STB_IMAGE_IMPLEMENTATION
#include "stdafx.h"

#include "engine.h"
#include "Log.h"

#include "TestScene/SceneTest.h"

int main() {
    engine::Core::init();
    engine::Core::start(new SceneTest);
    engine::Core::close();

    engine::Log::printInConsole();

    return 0;
}