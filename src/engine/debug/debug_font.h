#pragma once

#include "transform.h"
#include "render.h"
#include "texture.h"

void debug_font_init();
void debug_draw_char(Mesh * mesh, Transform2D * t, char c);
void debug_draw_string(Mesh * mesh, Transform2D * base, const char *text);
