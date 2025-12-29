#pragma once

#include <stdbool.h>

void input_init();
void input_reset();

bool input_key_down(int key);
bool input_key_pressed(int key); // Key pressed this frame
bool input_key_released(int key); // key released this frame

void input_mouse_position(double * x, double * y);
void input_mouse_delta(double * dx, double * dy);

