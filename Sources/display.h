#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <vector>

#include <Windows.h>
#undef min
#undef max

using uint = uint32_t;

struct DisplaySettings
{
    std::string name = "";

    int   width;
    int   height;
    int   frequency;
    float scale;
};

// https://github.com/imniko/SetDPI/blob/master/SetDpi.cpp
struct DisplayData
{
    LUID m_adapterId;
    int  m_targetID;
    int  m_sourceID;

    DisplayData()
    {
        m_adapterId = {};
        m_targetID = m_sourceID = -1;
    }
};

std::vector<DisplaySettings> list_display_settings();

// https://github.com/imniko/SetDPI/blob/master/SetDpi.cpp
std::vector<DisplayData> get_display_data();

bool update_resolution(int width, int height);

bool update_scale(float scale);

#endif // DISPLAY_H
