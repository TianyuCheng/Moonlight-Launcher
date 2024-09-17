#include <cassert>
#include <algorithm>
#include <cmrc/cmrc.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "font.h"
#include "application.h"

// Register the resource library
CMRC_DECLARE(fonts);

#define MAP_BUTTON(KEY_NO, BUTTON_NO, _UNUSED)                   \
    do {                                                         \
        io.AddKeyEvent(KEY_NO, gamepad.buttons[BUTTON_NO] != 0); \
    } while (0);

#define MAP_ANALOG(KEY_NO, AXIS_NO, _UNUSED, V0, V1)                        \
    do {                                                                    \
        float v = gamepad.axes[AXIS_NO];                                    \
        v       = (v - V0) / (V1 - V0);                                     \
        io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, std::clamp(v, 0.0f, 1.0f)); \
    } while (0);

// ---------------------------------------------------------------------------

static void on_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void on_scale(GLFWwindow* window, float xscale, float yscale)
{
    ImGuiIO& io = ImGui::GetIO();

    io.DisplayFramebufferScale = ImVec2(xscale, yscale);
}

static void on_error(int error, const char* description)
{
    spdlog::error("[GLFW] Error: {}\n", description);
}

// ---------------------------------------------------------------------------

void Application::run()
{
    setup();
    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        gamepad();
        tick();
        ImGui::Render();

        glViewport(0, 0, width, height);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cleanup();
}

void Application::tick()
{
    ImGui::ShowDemoWindow();
}

void Application::theme()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.TabRounding       = 16.f;
    style.FramePadding      = ImVec2(5, 5);
    style.FrameRounding     = 16.0f;
    style.GrabMinSize       = 5.0f;
    style.GrabRounding      = 3.0f;
    style.IndentSpacing     = 25.0f;
    style.ItemInnerSpacing  = ImVec2(8, 6);
    style.ItemSpacing       = ImVec2(12, 8);
    style.ScrollbarRounding = 8.0f;
    style.ScrollbarSize     = 15.0f;
    style.WindowPadding     = ImVec2(15, 15);
    style.WindowRounding    = 16.0f;
    style.ChildRounding     = 16.f;
    style.FrameBorderSize   = 0.0f;
    style.WindowBorderSize  = 4.0f;
    style.ChildBorderSize   = 0.0f;

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.4, 0.4, 0.4f, 1.0f);
}

void Application::setup()
{
    if (!glfwInit()) {
        spdlog::error("[GLFW] failed to initialize GLFW!");
        exit(1);
    }

    // create window
    glfwWindowHint(GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    // callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetErrorCallback(on_error);
    glfwSetKeyCallback(window, on_key);
    glfwSetWindowContentScaleCallback(window, on_scale);

    // OpenGL context
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // initialize OpenGL3
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    if (!ImGui_ImplOpenGL3_Init("#version 150")) {
        spdlog::error("[ImGui] Failed to initialize OpenGL3!");
        glfwTerminate();
        exit(-1);
    }

    // scale
    glfwGetWindowContentScale(window, &xscale, &yscale);

    // mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    // glfwSetCursorPos(window, 0.0, 0.0);

    // fonts
    fonts();

    // UI theme
    theme();

    // misc settings
    ImGuiIO& io    = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
}

void Application::cleanup()
{
    // imgui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // clean up
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::gamepad()
{
    auto update_joystick_events = [](int joystick) {
        ImGuiIO& io = ImGui::GetIO();

        GLFWgamepadstate gamepad;

        if (!glfwGetGamepadState(joystick, &gamepad))
            return;

        if (!glfwJoystickPresent(joystick)) {
            for (int i = 0; i < ImGuiNavInput_COUNT; ++i)
                io.NavInputs[i] = 0.0f;
            return;
        }

        int count;

        // Get the joystick axes
        const float* axes = glfwGetJoystickAxes(joystick, &count);
        if (axes) {
            // clang-format off
            MAP_ANALOG(ImGuiKey_GamepadLStickLeft,  GLFW_GAMEPAD_AXIS_LEFT_X,  0, -0.25f, -1.0f);
            MAP_ANALOG(ImGuiKey_GamepadLStickRight, GLFW_GAMEPAD_AXIS_LEFT_X,  0, +0.25f, +1.0f);
            MAP_ANALOG(ImGuiKey_GamepadLStickUp,    GLFW_GAMEPAD_AXIS_LEFT_Y,  1, -0.25f, -1.0f);
            MAP_ANALOG(ImGuiKey_GamepadLStickDown,  GLFW_GAMEPAD_AXIS_LEFT_Y,  1, +0.25f, +1.0f);
            MAP_ANALOG(ImGuiKey_GamepadRStickLeft,  GLFW_GAMEPAD_AXIS_RIGHT_X, 2, -0.25f, -1.0f);
            MAP_ANALOG(ImGuiKey_GamepadRStickRight, GLFW_GAMEPAD_AXIS_RIGHT_X, 2, +0.25f, +1.0f);
            MAP_ANALOG(ImGuiKey_GamepadRStickUp,    GLFW_GAMEPAD_AXIS_RIGHT_Y, 3, -0.25f, -1.0f);
            MAP_ANALOG(ImGuiKey_GamepadRStickDown,  GLFW_GAMEPAD_AXIS_RIGHT_Y, 3, +0.25f, +1.0f);
            // clang-format on
        }

        // Get the joystick buttons
        const unsigned char* buttons = glfwGetJoystickButtons(joystick, &count);
        if (buttons) {
            io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
            // clang-format off
            MAP_BUTTON(ImGuiKey_GamepadStart,     GLFW_GAMEPAD_BUTTON_START,        7);
            MAP_BUTTON(ImGuiKey_GamepadBack,      GLFW_GAMEPAD_BUTTON_BACK,         6);
            MAP_BUTTON(ImGuiKey_GamepadFaceLeft,  GLFW_GAMEPAD_BUTTON_X,            2);  // Xbox X, PS Square
            MAP_BUTTON(ImGuiKey_GamepadFaceRight, GLFW_GAMEPAD_BUTTON_B,            1); // Xbox B, PS Circle
            MAP_BUTTON(ImGuiKey_GamepadFaceUp,    GLFW_GAMEPAD_BUTTON_Y,            3);    // Xbox Y, PS Triangle
            MAP_BUTTON(ImGuiKey_GamepadFaceDown,  GLFW_GAMEPAD_BUTTON_A,            0);  // Xbox A, PS Cross
            MAP_BUTTON(ImGuiKey_GamepadDpadLeft,  GLFW_GAMEPAD_BUTTON_DPAD_LEFT,    13);
            MAP_BUTTON(ImGuiKey_GamepadDpadRight, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,   11);
            MAP_BUTTON(ImGuiKey_GamepadDpadUp,    GLFW_GAMEPAD_BUTTON_DPAD_UP,      10);
            MAP_BUTTON(ImGuiKey_GamepadDpadDown,  GLFW_GAMEPAD_BUTTON_DPAD_DOWN,    12);
            MAP_BUTTON(ImGuiKey_GamepadL1,        GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,  4);
            MAP_BUTTON(ImGuiKey_GamepadR1,        GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, 5);
            MAP_ANALOG(ImGuiKey_GamepadL2,        GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,   4, -0.75f, +1.0f);
            MAP_ANALOG(ImGuiKey_GamepadR2,        GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,  5, -0.75f, +1.0f);
            MAP_BUTTON(ImGuiKey_GamepadL3,        GLFW_GAMEPAD_BUTTON_LEFT_THUMB,   8);
            MAP_BUTTON(ImGuiKey_GamepadR3,        GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,  9);
            // clang-format on
        }
    };

    // query gamepad support
    for (uint joystick = GLFW_JOYSTICK_1; joystick <= GLFW_JOYSTICK_LAST; joystick++)
        if (glfwJoystickPresent(joystick) && glfwJoystickIsGamepad(joystick))
            update_joystick_events(joystick);
}

void Application::fonts()
{
    // for icon fonts
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0}; // Will not be copied by AddFont* so keep in scope.

    ImFontConfig config;
    config.OversampleH          = 2;
    config.OversampleV          = 2;
    config.GlyphExtraSpacing.x  = 0.0f;
    config.FontDataOwnedByAtlas = false;

    ImFontConfig main_cfg = config;
    main_cfg.MergeMode    = false;

    ImFontConfig icon_cfg = config;
    icon_cfg.MergeMode    = true;

    auto fs = cmrc::fonts::get_filesystem();

    auto  text_font = fs.open(TEXT_FONT_FILE);
    void* text_data = (char*)text_font.begin();

    auto  icon_font = fs.open(ICON_FONT_FILE);
    void* icon_data = (char*)icon_font.begin();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryTTF(text_data, text_font.size(), TEXT_FONT_SIZE * xscale, &main_cfg, io.Fonts->GetGlyphRangesDefault());
    io.Fonts->AddFontFromMemoryTTF(icon_data, icon_font.size(), ICON_FONT_SIZE * xscale, &icon_cfg, icons_ranges);
    io.Fonts->Build();

    ImGui_ImplOpenGL3_CreateFontsTexture();
}
