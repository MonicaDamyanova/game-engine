#pragma once

#include <GLFW/glfw3.h>

typedef struct window_config {
    int32_t position_x;
    int32_t position_y;
    uint32_t width;
    uint32_t height;
    const char* title;
} window_config;

GLFWwindow * window_init(window_config * config);
void window_destroy(GLFWwindow * window);
