#include "input.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdbool.h>

#include "logger.h"
#include "memory.h"

typedef struct input_state {
    // pressed and released MUST be cleared each frame.
    uint8_t down[GLFW_KEY_LAST + 1];
    uint8_t pressed[GLFW_KEY_LAST + 1];
    uint8_t released[GLFW_KEY_LAST + 1];

    // mouse
    int16_t mouse_x, mouse_y;
    uint8_t mouse_down[8];
    uint8_t mouse_pressed[8];
    uint8_t mouse_released[8];
} InputState;

InputState input_state = {0};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0 || key >= GLFW_KEY_LAST + 1) return;
    
    DEBUG("Key pressed: %s | Scancode: %d | Action: %d | Mods: %d", glfwGetKeyName(key, 0), scancode, action, mods);

    if (action == GLFW_PRESS) {
        input_state.down[key] = true;
        input_state.pressed[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        input_state.down[key] = false;
        input_state.released[key] = true;
    }
}

void input_init(GLFWwindow * window) {
    glfwSetKeyCallback(window, key_callback);
}

// Keyboard
bool input_key_down(int key) {
    return input_state.down[key]; 
}

bool input_key_pressed(int key) {
    return input_state.pressed[key];
}

bool input_key_released(int key) {
    return input_state.released[key];
}

void input_reset() {
    memory_set(&input_state.pressed,  0, sizeof(input_state.pressed));
    memory_set(&input_state.released, 0, sizeof(input_state.released));
}

// Mouse
void input_mouse_position(double * x, double * y) {

}

void input_mouse_delta(double * dx, double * dy) {

}
