
cmake_minimum_required(VERSION 3.12)

project(fodinga)

add_executable(fodinga main.cpp "main.cpp")

target_include_directories(fodinga PRIVATE deps/glm 
                                              deps/glfw/include
                                              deps/glew/include)

target_link_directories(fodinga PRIVATE deps/glfw/lib-vc2019
                                           deps/glew/lib/Release/x64)

target_link_libraries(fodinga PRIVATE glfw3.lib glew32.lib opengl32.lib)

add_custom_command(TARGET fodinga POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/glew/bin/Release/x64/glew32.dll" "${CMAKE_BINARY_DIR}/glew32.dll")