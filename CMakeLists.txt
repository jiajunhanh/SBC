cmake_minimum_required(VERSION 3.23)
project(SBC)

set(CMAKE_CXX_STANDARD 20)

add_executable(SBC
        include/shader_m.h
        include/camera.h
        include/sbc.h
        src/main.cpp
        src/sbc.cpp
        external/glad/src/gl.c)
target_include_directories(SBC PRIVATE include)

add_subdirectory(external/glm)
target_link_libraries(SBC glm::glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)
target_link_libraries(SBC glfw)

target_include_directories(SBC PRIVATE external/glad/include)

find_package(OpenGL REQUIRED)
target_link_libraries(SBC OpenGL::GL)