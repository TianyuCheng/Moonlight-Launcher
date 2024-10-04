cmake_minimum_required(VERSION 3.26)

# project name and version
project(Moonlight-Launcher VERSION 1.0 LANGUAGES C CXX)

# use only in-house cmake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/Configs")

# project settings
include(System)
include(Compiler)

# project resources
find_package(cmrc REQUIRED)
cmrc_add_resource_library(Font-Resources
    NAMESPACE fonts
    WHENCE ${PROJECT_SOURCE_DIR}/Assets/fonts
    Assets/fonts/Roboto-Regular.ttf
    Assets/fonts/Font-Awesome.ttf
    Assets/fonts/Prompt-Font.ttf)
set_target_properties(Font-Resources PROPERTIES FOLDER "Product")

# project sources
find_package(toml REQUIRED)
find_package(glfw REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
find_package(SetDPI REQUIRED)
find_package(iconfont REQUIRED)
find_package(promptfont REQUIRED)
add_executable(Moonlight-Launcher
    Sources/main.cpp
    Sources/logger.h
    Sources/logger.cpp
    Sources/display.h
    Sources/display.cpp
    Sources/application.h
    Sources/application.cpp)
target_link_libraries(Moonlight-Launcher PRIVATE Font-Resources)
target_link_libraries(Moonlight-Launcher PRIVATE iconfont promptfont)
target_link_libraries(Moonlight-Launcher PRIVATE toml glfw imgui spdlog SetDPI)
set_target_properties(Moonlight-Launcher PROPERTIES FOLDER "Product")

# project specific settings
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_compile_definitions(Moonlight-Launcher PRIVATE -DBUILD_WINDOWS_APPLICATION)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

# copy configuration file
add_custom_command(
    TARGET Moonlight-Launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/moonlight-launcher.toml
    ${CMAKE_BINARY_DIR}/moonlight-launcher.toml
    COMMENT "Copying configuration file!"
)
