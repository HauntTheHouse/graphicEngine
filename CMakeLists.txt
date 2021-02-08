cmake_minimum_required(VERSION 3.17)
project(graphicEngine)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_executable(graphicEngine main.cpp Engine.cpp Engine.h Shader.h Shader.cpp Object.cpp Object.h Camera.h Light.h Light.cpp)

target_link_libraries(graphicEngine GL glfw GLEW)