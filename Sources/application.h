#ifndef APPLICATION_H
#define APPLICATION_H

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

using uint = uint32_t;

struct Application
{
    virtual void run();

    virtual void tick();

    virtual void theme();

    virtual void setup();

    virtual void cleanup();

    virtual void gamepad();

    virtual void fonts();

    GLFWwindow* window    = nullptr;
    std::string title     = "Application";
    uint        width     = 0;
    uint        height    = 0;
    bool        decorated = true;
    float       xscale    = 1.0f;
    float       yscale    = 1.0f;

}; // end of class Application

#endif // APPLICATION_H
