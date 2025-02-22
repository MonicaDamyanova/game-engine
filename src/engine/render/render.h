#ifndef RENDER_H
#define RENDER_H

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

typedef struct render_state {
    GLFWwindow* window;
    int height;
    int width;
} Render_State;

void render_init(void);
void render_begin(void);
void render_end(void);
void render_quad(vec2 pos, vec2 size, vec4 colour);

#endif
