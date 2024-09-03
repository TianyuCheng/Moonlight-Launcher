#include <sstream>
#include <imgui.h>
#include "font.h"
#include "display.h"
#include "application.h"

struct MyApp : public Application
{
    explicit MyApp()
    {
        // display settings
        preset_display_settings.push_back(DisplaySettings{"HD", 1920, 1080, 60, 1.5f});
        preset_display_settings.push_back(DisplaySettings{"2K", 2560, 1440, 60, 1.5f});
        preset_display_settings.push_back(DisplaySettings{"4K", 3840, 2160, 60, 1.5f});
        preset_display_settings.push_back(DisplaySettings{"M3", 3024, 1964, 60, 1.5f});
        preset_display_settings.push_back(DisplaySettings{"iPad", 2388, 1668, 60, 1.5f});
        preset_display_settings.push_back(DisplaySettings{"iPhone", 2532, 1170, 60, 1.5f});
        preset_display_settings.push_back(DisplaySettings{"Intel", 3072, 1920, 60, 1.5f});
        preset_display_settings.push_back(DisplaySettings{"AOPEN", 3840, 1080, 60, 1.5f});

        // display settings
        supported_display_settings = list_display_settings();
    }

    void tick() override;

    bool is_up_pressed();
    bool is_down_pressed();
    bool is_enter_pressed();
    bool is_next_tab_pressed();
    bool is_prev_tab_pressed();
    bool is_exit_pressed();

    void render_pages();
    void render_displays(const char* name, const std::vector<DisplaySettings>& display_settings);

    std::vector<DisplaySettings> preset_display_settings{};
    std::vector<DisplaySettings> supported_display_settings{};
};

void MyApp::tick()
{
    render_pages();
    glfwFocusWindow(window);
}

bool MyApp::is_up_pressed()
{
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadDpadUp));
}

bool MyApp::is_down_pressed()
{
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadDpadDown));
}

bool MyApp::is_enter_pressed()
{
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadFaceDown));
}

bool MyApp::is_next_tab_pressed()
{
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadR1)) ||
           (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)) && !ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftShift)));
}

bool MyApp::is_prev_tab_pressed()
{
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadL1)) ||
           (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)) && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftShift)));
}

bool MyApp::is_exit_pressed()
{
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadStart));
}

void MyApp::render_pages()
{
    static int tab_index = 0;
    static int tab_count = 2;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Windows Desktop Resizer", 0, flags);

    if (ImGui::BeginTabBar("Resolutions")) {

        if (ImGui::BeginTabItem(ICON_FA_GIFT, nullptr, tab_index == 0 ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None)) {
            render_displays("##Presets", preset_display_settings);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(ICON_FA_LIST_UL, nullptr, tab_index == 1 ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None)) {
            render_displays("##Supported", supported_display_settings);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    // next tab
    if (is_next_tab_pressed()) {
        tab_index = (tab_index + 1) % tab_count;
    }

    // prev tab
    if (is_prev_tab_pressed()) {
        tab_index = (tab_index - 1) % tab_count;
    }

    if (is_exit_pressed()) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    ImGui::End();
}

void MyApp::render_displays(const char* name, const std::vector<DisplaySettings>& display_settings)
{
    static int sel_index = 0;

    bool  execute = false;
    float height  = ImGui::GetContentRegionAvail().y;

    ImGui::PushItemWidth(-1);
    if (ImGui::ListBoxHeader(name, ImVec2(-1, height))) {
        for (size_t i = 0; i < display_settings.size(); i++) {
            const auto& settings = display_settings.at(i);
            const bool  selected = sel_index == i;

            std::stringstream ss;
            ss << ICON_FA_DESKTOP << " " << settings.width << "x" << settings.height << "@" << settings.frequency;
            if (!settings.name.empty())
                ss << " (" << settings.name << ")";

            if (ImGui::Selectable(ss.str().c_str(), selected)) {
                sel_index = i;
                execute   = true;
            }

            // Scroll to ensure the selected item is visible
            if (i == sel_index) {
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + ImGui::GetCursorPosY(), 0.5f);
            }
        }
        ImGui::ListBoxFooter();
    }
    ImGui::PopItemWidth();

    if (is_up_pressed()) {
        sel_index = (sel_index - 1 + display_settings.size()) % display_settings.size();
    }

    if (is_down_pressed()) {
        sel_index = (sel_index + 1) % display_settings.size();
    }

    if (is_enter_pressed()) {
        execute = true;
    }

    if (execute) {
        auto& settings = display_settings.at(sel_index);
        update_scale(settings.scale);
        update_resolution(settings.width, settings.height);
    }
}

#ifdef USE_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, const char* argv[])
#endif
{
    if (!glfwInit()) {
        spdlog::error("[GLFW] failed to initialize GLFW!");
        exit(1);
    }

    // query primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        spdlog::error("[GLFW] Failed to get primary monitor!");
        glfwTerminate();
        exit(-1);
    }

    // query video mode
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) {
        spdlog::error("[GLFW] Failed to get video mode!");
        glfwTerminate();
        exit(-1);
    }

    // display info
    spdlog::info("Current resolution: {}x{}", mode->width, mode->height);
    spdlog::info("Refresh rate: {} Hz", mode->refreshRate);

    // application
    MyApp app;
    app.width     = mode->width;
    app.height    = mode->height;
    app.decorated = false;
    app.title     = "Virtual Desktop Resizer";
    app.run();
}
