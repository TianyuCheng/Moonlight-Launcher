#ifndef PATH_H
#define PATH_H

#include <Windows.h>
#include <shlobj.h>
#include <optional>
#include <string>

inline std::optional<std::string> get_app_config_path(const std::string& app = "Moonlight-Launcher")
{
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\" + app;
    }
    return std::nullopt;
}

#endif // PATH_H
