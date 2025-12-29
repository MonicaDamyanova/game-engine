#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdbool.h>

typedef struct {
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    uint32_t index_count;
} Mesh;


bool render_init(GLFWwindow* window);
void render_destroy();

void render_mesh(Mesh * mesh, mat4 * transform); 

void render_begin();
void render_end();

uint32_t render_get_height();
uint32_t render_get_width();
