cmake_minimum_required(VERSION 3.21)
project(testing_cmake)

message(${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -march=native -mavx")
set(CMAKE_CXX_FLAGS_RELEASE "-march=native -mavx -flto -mfpmath=sse -O2 -static -Wl,-Bdynamic,-lgcc_s,-Bstatic")
#set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -Wall")
#set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/libs/include)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRC_FiILES "src/*.h" "src/*.cpp")

set(EXE_NAME "game")

add_executable(${EXE_NAME} ${SRC_FiILES})
if(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
    message("Use MinGW configuration")    
    target_link_libraries(${EXE_NAME} ${CMAKE_SOURCE_DIR}/libs/lib/libglew32.a)
    target_link_libraries(${EXE_NAME} ${CMAKE_SOURCE_DIR}/libs/lib/libglfw3.a)
    target_link_libraries(${EXE_NAME} ${CMAKE_SOURCE_DIR}/libs/lib/libassimp.dll.a)
    target_link_libraries(${EXE_NAME} OpenGL::GL)
elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 16 2019" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 17 2022")
    message("Use Visual Studio configuration")   
endif()