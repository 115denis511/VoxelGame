#define STB_IMAGE_IMPLEMENTATION
#include "stdafx.h"

#include "engine.h"
#include "Log.h"

int main() {
    engine::Core::init();
    engine::Core::start();
    engine::Core::close();

    engine::Log::printInConsole();

    return 0;
}