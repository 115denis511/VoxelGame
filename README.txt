# Requared libs
    - GLFW
    - GLEW
    - stb_image
        place in ..\libs\include
    - assimp 3.3.1
        Off flag ASSIMP_BUILD_TESTS before cmake generation!

# How create project 
    (MinGW) - run in terminal:
        cmake -G "MinGW Makefiles" -B build

# How compile
    (MinGW) - run in terminal
        cmake --build build
