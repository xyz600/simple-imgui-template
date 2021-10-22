cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(solver CXX)

# main program
add_executable(solver main.cpp)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(solver PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(solver PRIVATE imgui::imgui)

find_package(OpenGL REQUIRED)
target_link_libraries(solver PRIVATE OpenGL)
