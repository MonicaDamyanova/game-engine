#include <glad/glad.c>
#include <cglm/cglm.h>

#include "../global.h"

#include "render.h"
#include "render_internal.h"

static Render_State_Internal state = {0};

void render_init(void) {
    global.render.width = 1000;
    global.render.height = 700;
    global.render.window = render_init_window(global.render.width, global.render.height);

    render_init_quad(&state.vao_quad, &state.vbo_quad, &state.ebo_quad);
    render_init_shaders(&state);
    render_init_color_texture(&state.texture_color);
}

void render_begin(void) {
    glClearColor(0.5, 0.5, 0.3, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void render_end(void) {
    glfwSwapBuffers(global.render.window);
    glfwPollEvents();
}

void render_quad(vec2 pos, vec2 size, vec4 color) {
    glUseProgram(global.render.shader_default);

    mat4 model;
    glm_mat4_identity(model);

    vec3 translate = {pos[0], pos[1], 0};
    vec3 scale = {size[0], size[1], 1};

    glm_translate(model, translate);
    glm_scale(model, scale);

    glUniformMatrix4fv(glGetUniformLocation(global.render.shader_default, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform4fv(glGetUniformLocation(global.render.shader_default, "color"), 1, color);

    glBindVertexArray(state.vao_quad);

    glBindTexture(GL_TEXTURE_2D, state.texture_color);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}
