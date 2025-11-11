#include <glad/glad.h>
#include <stdio.h>

#include "../logger.h"
#include "../io/io.h"

#include "render_internal.h"

uint32_t render_shader_create(const char* path_vert, const char* path_frag) {
    int success;
    char log[512];

    INFO("Attempt reading %s.", path_vert);
    File file_vertex = io_file_read(path_vert);
    INFO("Completed io_file_read successfully.");
    if (!file_vertex.is_valid) {
        FATAL("Error reading shader: %s\n", path_vert); exit(1);
    }
    INFO("Successfully read %s.", path_vert);

    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const char* const *)&file_vertex, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, log);
        FATAL("Error compiling vertex shader.\n%s", log); exit(1);
    }

    File file_fragment = io_file_read(path_frag);
    if (!file_fragment.is_valid) {
        FATAL("Error reading shader: %s", path_frag); exit(1);
    }


    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const char* const *)&file_fragment, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, log);
        WARN("Error compiling fragment shader.\n%s", log);
    }

    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, log);
        FATAL("Error linking shader program.\n%s", log); exit(1);
    }

    free(file_vertex.data);
    free(file_fragment.data);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    INFO("Successfully created a shader program.");

    return shader_program;
}
