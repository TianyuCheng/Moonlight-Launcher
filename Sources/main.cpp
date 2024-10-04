#include <fstream>
#include <sstream>
#include <imgui.h>
#include <filesystem>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

#include "font.h"
#include "display.h"
#include "application.h"

struct AppLauncher
{
    std::string name     = "";
    std::string script   = "";
    bool        elevated = false;
    std::string commands = "";

    void launch()
    {
        spdlog::info("Launch {}", name);

        prepare();
        execute();
    }

    void prepare()
    {
        auto scriptfile = std::filesystem::current_path() / script;
        // if (std::filesystem::exists(scriptfile)) return;

        std::ofstream of(scriptfile, std::ios::out);
        of << commands;
        of.close();
    }

    void execute()
    {
        spdlog::info("script {}", script);
        HINSTANCE result = ShellExecuteA(
            nullptr,
            elevated ? "runas" : "open",
            script.c_str(),
            NULL,
            NULL,
            SW_SHOWNORMAL);

        if ((long long)(result) <= 32) {
            spdlog::error("Failed to execute command!");
            exit(1);
        } else {
            spdlog::info("Command executed successfully!");
        }
    }
};

struct MyApp : public Application
{
    explicit MyApp()
    {
        init();

        // display settings
        supported_display_settings = list_display_settings();
    }

    void init();
    void tick() override;

    bool is_up_pressed();
    bool is_down_pressed();
    bool is_enter_pressed();
    bool is_next_tab_pressed();
    bool is_prev_tab_pressed();
    bool is_exit_pressed();

    void render_vtabs();
    void render_tab_button(const char* label, int tab_index, int& selected_tab);
    void render_exit_button(const char* label);
    void render_displays(const char* name, const std::vector<DisplaySettings>& display_settings);
    void render_launcher();
    void render_helpmenu();

    std::vector<DisplaySettings> preset_display_settings{};
    std::vector<DisplaySettings> supported_display_settings{};
    std::vector<AppLauncher>     application_launchers{};

    int tab_index = 0;
    int tab_count = 4;
};

void MyApp::init()
{
    auto config_file = std::filesystem::current_path() / "moonlight-launcher.toml";
    if (!std::filesystem::exists(config_file)) {
        spdlog::info("Toml config file does not exist!");
        return;
    }

    toml::parse_result result = toml::parse_file(config_file.c_str());
    if (!result) {
        spdlog::error("Failed to parse toml config file!");
        exit(1);
    }

    toml::table table = std::move(result).table();

    // parse resolutions
    auto resolutions = table["resolutions"];
    for (auto& elem : *resolutions.as_array()) {
        auto* item   = elem.as_table();
        auto  name   = item->get("name")->value_or("");
        auto  freq   = item->get("freq")->value_or(60);
        auto  scale  = item->get("scale")->value_or(1.0f);
        auto  width  = item->get("width")->value_or(0);
        auto  height = item->get("height")->value_or(0);

        // sanity check
        if (scale < 1.0f) {
            spdlog::error("[resolutions] expect scale >= 1.0f!");
            exit(1);
        }

        // sanity check
        if (width <= 0 || height <= 0) {
            spdlog::error("[resolutions] expect width/height > 0!");
            exit(1);
        }

        // sanity check
        if (freq <= 0) {
            spdlog::error("[resolutions] expect frequency > 0!");
            exit(1);
        }

        preset_display_settings.push_back(
            DisplaySettings{name, width, height, freq, scale});
    }

    // parse applications
    auto apps = table["apps"];
    for (auto& elem : *apps.as_array()) {
        auto* item     = elem.as_table();
        auto  name     = item->get("name")->value_or<std::string>("");
        auto  elevated = item->get("elevated")->value_or(false);
        auto  commands = item->get("commands")->value_or<std::string>("");

        // sanity check
        if (name.empty()) {
            spdlog::error("[apps] expect non-empty name!");
            exit(1);
        }

        // sanity check
        if (commands.empty()) {
            spdlog::error("[apps] expect non-empty commands!");
            exit(1);
        }

        auto script = name + ".bat";
        std::replace(script.begin(), script.end(), ' ', '_');
        application_launchers.push_back(
            AppLauncher{name, script, elevated, commands});
    }
}

void MyApp::tick()
{
    // glfwFocusWindow(window);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Moonlight Launcher", 0, flags);
    {
        render_vtabs();
    }
    ImGui::End();
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
           (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)) && !ImGui::GetIO().KeyShift);
}

bool MyApp::is_prev_tab_pressed()
{
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadL1)) ||
           (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)) && ImGui::GetIO().KeyShift);
}

bool MyApp::is_exit_pressed()
{
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadStart));
}

void MyApp::render_vtabs()
{
    ImGui::BeginChild("Tab Buttons", ImVec2(150, 0), true);
    {
        render_tab_button(ICON_FA_HOME, 0, tab_index);
        render_tab_button(ICON_FA_LAPTOP, 1, tab_index);
        render_tab_button(ICON_FA_CUBE, 2, tab_index);
        render_tab_button(ICON_FA_INFO, 3, tab_index);
        render_exit_button(ICON_FA_POWER_OFF);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Tab Content", ImVec2(0, 0), true);
    {
        // clang-format off
        switch (tab_index)
        {
            case 0: render_displays("##Presets", preset_display_settings);      break;
            case 1: render_displays("##Supported", supported_display_settings); break;
            case 2: render_launcher();                                          break;
            case 3: render_helpmenu();                                          break;
        }
        // clang-format on
    }
    ImGui::EndChild();

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
}

void MyApp::render_tab_button(const char* label, int tab_index, int& selected_tab)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    // adjust selectable padding
    ImVec2 txt_size = ImGui::CalcTextSize(label);
    ImVec2 row_size = ImGui::GetContentRegionAvail();
    ImVec2 sel_size = ImVec2(0, row_size.x);
    float  offset_x = std::max(0.0f, (row_size.x - txt_size.x) / 2.0f);
    float  offset_y = std::max(0.0f, (row_size.x - txt_size.y) / 2.0f);

    // render selectable placeholder
    std::string ID = "##" + std::string(label) + "-" + std::to_string(tab_index);
    if (ImGui::Selectable(ID.c_str(), selected_tab == tab_index, 0, sel_size))
        selected_tab = tab_index;

    // fill selectable content
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset_y);
    ImGui::TextUnformatted(label);

    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}

void MyApp::render_exit_button(const char* label)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    // adjust selectable padding
    ImVec2 txt_size = ImGui::CalcTextSize(label);
    ImVec2 row_size = ImGui::GetContentRegionAvail();
    ImVec2 sel_size = ImVec2(0, row_size.x);
    float  offset_x = std::max(0.0f, (row_size.x - txt_size.x) / 2.0f);
    float  offset_y = std::max(0.0f, (row_size.x - txt_size.y) / 2.0f);

    // render selectable placeholder
    if (ImGui::Selectable("##exit", false, 0, sel_size))
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    // fill selectable content
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset_y);
    ImGui::TextUnformatted(label);

    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}

void MyApp::render_displays(const char* name, const std::vector<DisplaySettings>& display_settings)
{
    static int sel_index = 0;

    bool  execute = false;
    bool  is_key  = is_up_pressed() || is_down_pressed();
    float height  = ImGui::GetContentRegionAvail().y;

    ImGui::PushItemWidth(-1);
    if (ImGui::ListBoxHeader(name, ImVec2(-1, height))) {
        for (size_t i = 0; i < display_settings.size(); i++) {
            const auto& settings = display_settings.at(i);
            const bool  selected = sel_index == i;

            std::stringstream ss;
            ss << " " << ICON_FA_LAPTOP << " " << settings.width << "x" << settings.height << "@" << settings.frequency << " Hz";
            if (!settings.name.empty())
                ss << " (" << settings.name << ")";
            ImGui::Selectable(ss.str().c_str(), selected);

            if (ImGui::IsItemHovered()) {
                sel_index = i;
                execute   = false;
            }

            if (ImGui::IsItemClicked()) {
                sel_index = i;
                execute   = true;
            }

            if (is_key && sel_index == i) {
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + ImGui::GetCursorPosY(), 0.5f);
            }
        }
        ImGui::ListBoxFooter();
    }
    ImGui::PopItemWidth();

    if (is_up_pressed()) {
        sel_index = (sel_index - 1 + display_settings.size()) % display_settings.size();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (is_down_pressed()) {
        sel_index = (sel_index + 1) % display_settings.size();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

void MyApp::render_launcher()
{
    static int sel_index = 0;

    bool  execute = false;
    bool  is_key  = is_up_pressed() || is_down_pressed();
    float height  = ImGui::GetContentRegionAvail().y;

    ImGui::PushItemWidth(-1);
    if (ImGui::ListBoxHeader("#app-launcher", ImVec2(-1, height))) {
        for (size_t i = 0; i < application_launchers.size(); i++) {
            const auto& settings = application_launchers.at(i);
            const bool  selected = sel_index == i;

            std::stringstream ss;
            ss << " " << ICON_FA_CUBE << " " << settings.name;
            ImGui::Selectable(ss.str().c_str(), selected);

            if (ImGui::IsItemHovered()) {
                sel_index = i;
                execute   = false;
            }

            if (ImGui::IsItemClicked()) {
                sel_index = i;
                execute   = true;
            }

            if (is_key && sel_index == i) {
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + ImGui::GetCursorPosY(), 0.5f);
            }
        }
        ImGui::ListBoxFooter();
    }
    ImGui::PopItemWidth();

    if (is_up_pressed()) {
        sel_index = (sel_index - 1 + application_launchers.size()) % application_launchers.size();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (is_down_pressed()) {
        sel_index = (sel_index + 1) % application_launchers.size();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (is_enter_pressed()) {
        execute = true;
    }

    if (execute) {
        auto& launcher = application_launchers.at(sel_index);
        launcher.launch();
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void MyApp::render_helpmenu()
{
    // clang-format off
    static std::vector<std::tuple<std::string, std::string, std::string>> shortcuts = {
        {"Next Tab",  PF_KEYBOARD_TAB,                   PF_SONY_RIGHT_SHOULDER,    },
        {"Prev Tab",  PF_KEYBOARD_SHIFT PF_KEYBOARD_TAB, PF_SONY_LEFT_SHOULDER,     },
        {"Next Item", PF_KEYBOARD_DOWN,                  PF_DPAD_DOWN,              },
        {"Prev Item", PF_KEYBOARD_UP,                    PF_DPAD_UP,                },
        {"Select",    PF_KEYBOARD_ENTER,                 PF_SONY_A,                 },
        {"Exit",      PF_KEYBOARD_ESCAPE,                PF_SONY_DUALSENSE_OPTIONS, },
    };
    // clang-format on

    float width  = ImGui::GetContentRegionAvail().x - 16.0f;
    float height = ImGui::GetContentRegionAvail().y;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.0f);
    if (ImGui::ListBoxHeader("#shortcuts", ImVec2(width, height))) {
        if (ImGui::BeginTable("Shortcuts##table", 3)) {
            ImGui::TableHeadersRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Action");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Keyboard");
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("Gamepad");
            ImGui::TableNextRow();
            ImGui::Separator();

            for (auto& row : shortcuts) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", std::get<0>(row).c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", std::get<1>(row).c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", std::get<2>(row).c_str());
            }
            ImGui::EndTable();
        }
    }
}

#ifdef BUILD_WINDOWS_APPLICATION
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

    // gamepad info
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
        if (glfwJoystickPresent(jid)) {
            if (glfwJoystickIsGamepad(jid)) {
                const char* name = glfwGetGamepadName(jid);
                spdlog::info("Gamepad {}: {}", jid, name);
            }
        }
    }

    // application
    MyApp app;
    app.width     = mode->width;
    app.height    = mode->height;
    app.decorated = false;
    app.title     = "Moonlight Launcher";
    app.run();
}
