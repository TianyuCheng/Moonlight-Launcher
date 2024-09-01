cmake_minimum_required(VERSION 3.26)

# project name and version
project(Windows-Desktop-Resizer VERSION 1.0 LANGUAGES C CXX)

# use only in-house cmake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/Configs")

# project settings
include(System)
include(Compiler)

# project resources
find_package(cmrc REQUIRED)
cmrc_add_resource_library(font-resources
    NAMESPACE fonts
    WHENCE ${PROJECT_SOURCE_DIR}/Assets/fonts
    Assets/fonts/Roboto-Regular.ttf)

# project sources
find_package(glfw REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
find_package(SetDPI REQUIRED)
add_executable(Windows-Desktop-Resizer
    Sources/main.cpp
    Sources/display.h
    Sources/display.cpp
    Sources/application.h
    Sources/application.cpp)
target_link_libraries(Windows-Desktop-Resizer PRIVATE glfw imgui spdlog SetDPI)
target_link_libraries(Windows-Desktop-Resizer PRIVATE font-resources)
