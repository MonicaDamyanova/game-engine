#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdbool.h>

#define MAX_SHADERS 64
#define INVALID_SHADER 0
#define DEFAULT_SHADER 1

typedef uint32_t ShaderHandle;

enum ShaderUniform {
    UNIFORM_MODEL,
    UNIFORM_PROGECTION,
    UNIFORM_UVS,
    UNIFORM_TEXTUREID,
    UNIFORM_COLOUR,
    TOTAL_CACHED_UNIFORMS
};

typedef struct {
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    uint32_t index_count;
} Mesh;

bool render_init(GLFWwindow* window);
void render_destroy();

ShaderHandle render_shader_add(const char * path_vert, const char * path_frag);
void render_shader_use(ShaderHandle handle);

//void render_shader_set_uniform_m4_cached(ShaderHandle, ShaderUniform loc, mat4 value);
//void render_shader_set_uniform_m4(ShaderHandle handle, const char * name, mat4 value);
void render_shader_set_uniform_i(ShaderHandle handle, const char * name, int value);
void render_shader_set_uniform_f(ShaderHandle handle, const char * name, float value);
void render_shader_set_uniform_v2(ShaderHandle handle, const char * name, vec2 value);
void render_shader_set_uniform_v3(ShaderHandle handle, const char * name, vec3 value);
void render_shader_set_uniform_v4(ShaderHandle handle, const char * name, vec4 value);
void render_shader_set_uniform_m3(ShaderHandle handle, const char * name, mat3 value);
void render_shader_set_uniform_m4(ShaderHandle handle, const char * name, mat4 value);

void render_upload_mesh(Mesh * mesh, float * vertices, uint32_t * indices, uint32_t vertices_size, uint32_t indices_size);
void render_mesh(Mesh * mesh, mat4 transform); 
void render_mesh_uv(Mesh * mesh, mat4  transform, vec4 uv_rect);

uint32_t render_get_height();
uint32_t render_get_width();
