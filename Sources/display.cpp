#include <algorithm>
#include <DpiHelper.h>
#include <spdlog/spdlog.h>

#include "logger.h"
#include "display.h"

using uint = uint32_t;

std::vector<DisplaySettings> list_display_settings()
{
    std::vector<DisplaySettings> display_settings{};

    auto update_display_settings = [&](const DisplaySettings& settings) {
        for (auto& config : display_settings) {
            if (config.width == settings.width && config.height == settings.height) {
                config.frequency = std::max(config.frequency, settings.frequency);
                return;
            }
        }
        display_settings.push_back(settings);
    };

    int modeNum = 0;

    DEVMODE dm;
    ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(dm);

    while (EnumDisplaySettings(NULL, modeNum, &dm)) {
        DisplaySettings settings{};
        settings.width     = dm.dmPelsWidth;
        settings.height    = dm.dmPelsHeight;
        settings.frequency = dm.dmDisplayFrequency;
        settings.scale     = dm.dmScale;
        modeNum++;
        update_display_settings(settings);
    }

    std::sort(display_settings.begin(), display_settings.end(), [&](const auto& lhs, const auto& rhs) {
        return lhs.width != rhs.width ? (lhs.width > rhs.width) : (lhs.height > rhs.height);
    });

    return display_settings;
}

// https://github.com/imniko/SetDPI/blob/master/SetDpi.cpp
std::vector<DisplayData> get_display_data()
{
    std::vector<DisplayData>             displayDataCache;
    std::vector<DISPLAYCONFIG_PATH_INFO> pathsV;
    std::vector<DISPLAYCONFIG_MODE_INFO> modesV;

    int flags = QDC_ONLY_ACTIVE_PATHS;
    if (false == DpiHelper::GetPathsAndModes(pathsV, modesV, flags)) {
        Logger::error("DpiHelper::GetPathsAndModes() failed");
    }

    displayDataCache.resize(pathsV.size());
    int idx = 0;
    for (const auto& path : pathsV) {
        // get display name
        auto adapterLUID = path.targetInfo.adapterId;
        auto targetID    = path.targetInfo.id;
        auto sourceID    = path.sourceInfo.id;

        DISPLAYCONFIG_TARGET_DEVICE_NAME deviceName;
        deviceName.header.size      = sizeof(deviceName);
        deviceName.header.type      = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        deviceName.header.adapterId = adapterLUID;
        deviceName.header.id        = targetID;
        if (ERROR_SUCCESS != DisplayConfigGetDeviceInfo(&deviceName.header)) {
            Logger::error("DisplayConfigGetDeviceInfo() failed!");
        } else {
            std::wstring nameString = std::to_wstring(idx) + std::wstring(L". ") + deviceName.monitorFriendlyDeviceName;
            if (DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL == deviceName.outputTechnology) {
                nameString += L"(internal display)";
            }
            DisplayData dd = {};
            dd.m_adapterId = adapterLUID;
            dd.m_sourceID  = sourceID;
            dd.m_targetID  = targetID;

            displayDataCache[idx] = dd;
        }
        idx++;
    }
    return displayDataCache;
}

bool update_resolution(int width, int height)
{
    // initialize DEVMODE structure
    DEVMODE dm;
    ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(dm);

    // get current display settings
    if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm)) {
        Logger::error("Could not get current display settings!");
        return false;
    }

    // set new display resolution
    dm.dmPelsWidth  = width;
    dm.dmPelsHeight = height;
    dm.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;

    // apply the new settings
    DWORD flags  = CDS_UPDATEREGISTRY | CDS_GLOBAL;
    LONG  result = ChangeDisplaySettings(&dm, flags);

    if (result != DISP_CHANGE_SUCCESSFUL) {
        Logger::error("Display resolution change failed.");
        return false;
    }

    Logger::info("Display resolution changed to {}x{}.", width, height);
    return true;
}

bool update_scale(float scale)
{
    uint displayIndex = 0;
    uint dpiToSet     = static_cast<uint32_t>(scale * 100.0);

    auto displayDataCache = get_display_data();

    bool success = DpiHelper::SetDPIScaling(displayDataCache[displayIndex].m_adapterId, displayDataCache[displayIndex].m_sourceID, dpiToSet);
    if (!success) {
        Logger::error("DpiHelper::SetDPIScaling() failed!");
        return false;
    }

    return true;
}
