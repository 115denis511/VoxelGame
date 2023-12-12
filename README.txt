# Requared libs
    - GLFW
        place libglfw3.a in ..\libs\lib
        place include folder in ..\libs\include (must be ..\libs\include\GLFW)
    - GLEW
        place libglew32.dll.a, libglew32.a in ..\libs\lib
        place include folder in ..\libs\include (must be ..\libs\include\GL)
    - glm
        place include folder in ..\libs\include (must be ..\libs\include\glm)
    - stb_image.h
        place in ..\libs\include
    - assimp 5.2
        Off flag ASSIMP_BUILD_TESTS before cmake generation!
        place libassimp.dll.a in ..\libs\lib
        place include folder in ..\libs\include (must be ..\libs\include\assimp)
        place libassimp-5.dll in ..\bin

# How compile
    (MinGW) - run in terminal
        ./build_MinGW_debug.bat
