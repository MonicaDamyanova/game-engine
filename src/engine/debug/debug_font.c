#include "debug_font.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "transform.h"
#include "render.h"
#include "texture.h"
#include "logger.h"

typedef struct {
    Texture texture;
    int glyph_width;
    int glyph_height;
    int atlas_width;
    int atlas_height;
    ShaderHandle shader_default;
} DebugFont;

DebugFont font = {0};

void debug_font_init() {
    font.texture = texture_load("../assets/DEBUG_font-002.png");
    font.glyph_width = font.glyph_height = 8;
    font.atlas_width = 256;
    font.atlas_height = 24;
    font.shader_default = render_shader_add("../shaders/default.vert", "../shaders/debug_font.frag");
}

void debug_font_uv(char c, float * u0, float * v0, float * u1, float * v1) {
    int index = c - 33;
    if (index < 0) index = 0;

    int x = index % 32;
    int y = index / 32;
    
    //DEBUG("Char indices: [%d, %d]", x, y);

    *u0 = (x * font.glyph_width) / (float)font.atlas_width; 
    *v0 = (y * font.glyph_height) / (float)font.atlas_height;
    *u1 = ((x + 1) * font.glyph_width) / (float)font.atlas_width;
    *v1 = ((y + 1) * font.glyph_height) / (float)font.atlas_height;
}

void debug_draw_char(Mesh * mesh, Transform2D * t, char c) {
    //DEBUG("Char: %c", c);
    float u0, v0, u1, v1; 
    debug_font_uv(c, &u0, &v0, &u1, &v1);
    vec4 UVs = {u1, v0, u0, v1};
    vec4 colour = {0, 0, 0, 1.0};
   
    //DEBUG("Created UVs: %f, %f, %f, %f", UVs[0], UVs[1], UVs[2], UVs[3]);

    render_shader_use(DEFAULT_SHADER+1);
    mat4 matrix;
    transform_matrix(t, matrix);

    render_shader_use(font.shader_default);
  
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texture_bind(&(font.texture), 0);
    render_shader_set_uniform_v4(font.shader_default, "u_colour", colour);
    render_mesh_uv(mesh, matrix, UVs);
}

void debug_draw_string(Mesh * mesh, Transform2D * base, const char *text) {
    Transform2D t = *base;

    float advance = t.scale[0];

    for (const char *p = text; *p; ++p) {
        if (*p == '\n') {
            t.position[0] = base->position[0];
            t.position[1] -= 8.0f * t.scale[1];
            continue;
        }

        debug_draw_char(mesh, &t, *p);
        t.position[0] += advance;
    }
}
