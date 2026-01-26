#include "render.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdbool.h>
#include <stdio.h>

#include "logger.h"
#include "filesystem.h"

struct render_state {
    uint32_t width;
    uint32_t height;
 
    uint32_t shader_default;
    uint32_t texture_colour;
    
    mat4 projection;
};

static struct render_state state = {0};

uint32_t render_shader_create(const char* path_vert, const char* path_frag) {
    int success;
    char log[512];

    INFO("Attempt reading %s.", path_vert);
    File file_vertex = io_file_read(path_vert);
    INFO("Completed io_file_read successfully.");
    if (!file_vertex.is_valid) {
        FATAL("Error reading shader: %s\n", path_vert);
        return 0;
    }
    INFO("Successfully read %s.", path_vert);

    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const char* const *)&file_vertex, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, log);
        FATAL("Error compiling vertex shader.\n%s", log);
        return 0;
    }

    File file_fragment = io_file_read(path_frag);
    if (!file_fragment.is_valid) {
        FATAL("Error reading shader: %s", path_frag);
        return 0;
    }


    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const char* const *)&file_fragment, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, log);
        WARN("Error compiling fragment shader.\n%s", log);
    }

    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, log);
        FATAL("Error linking shader program.\n%s", log);
        return 0;
    }

    free(file_vertex.data);
    free(file_fragment.data);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    INFO("Successfully created a shader program.");

    return shader_program;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    struct render_state * state = glfwGetWindowUserPointer(window);

    glViewport(0, 0, width, height);
    state->width = width;
    state->height = height;

    glm_ortho(0.0f, (float) width, 0.0f, (float) height, -2.0f, 2.0f, state->projection);

    glUniformMatrix4fv(
        glGetUniformLocation(state->shader_default, "projection"),
        1, GL_FALSE, &state->projection[0][0]
    );
}

bool render_init(GLFWwindow* window) {
    glfwGetWindowSize(window, &state.width, &state.height);

    DEBUG("Window width: %d", state.width);
    DEBUG("Window height: %d", state.height);

    glfwSetWindowUserPointer(window, &state);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        FATAL("Failed to initialize GLAD.");
        return false;
    }

    // Add any openGL state here, maybe have a config struct for this

    // Init default shaders
    state.shader_default = render_shader_create("../shaders/default.vert", "../shaders/default.frag");
    //state.shader_default = render_shader_create("../shaders/debug_font.vert", "../shaders/debug_font.frag");

    glm_ortho(0.0f, (float) state.width, 0.0f, (float) state.height, -2.0f, 2.0f, state.projection);

    glUseProgram(state.shader_default);
    glUniformMatrix4fv(
        glGetUniformLocation(state.shader_default, "projection"),
        1, GL_FALSE, &state.projection[0][0]
    );

    // Textures
    glGenTextures(1, &state.texture_colour);
    glBindTexture(GL_TEXTURE_2D, state.texture_colour);

    uint8_t solid_white[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, solid_white);

    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    INFO("Vendor:   %s", glGetString(GL_VENDOR));
    INFO("Renderer: %s", glGetString(GL_RENDERER));
    INFO("Version:  %s", glGetString(GL_VERSION));

    return true;
}

void render_upload_mesh(Mesh * mesh, float * vertices, uint32_t * indices, uint32_t vertices_size, uint32_t indices_size) {
    DEBUG("Initializing the square.");

    DEBUG("Memory address of VAO: %d", &mesh->vao);
    DEBUG("Memory address of VBO: %d", &mesh->vbo);
    DEBUG("Memory address of EBO: %d", &mesh->ebo);

    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);

    DEBUG("Generated buffers and arrays.");

    glBindVertexArray(mesh->vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

    DEBUG("Added buffer data.");

    // xyz
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    DEBUG("Described the XYZ data for the VAO.");

    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    DEBUG("Described the UV data for the VAO.");

    glBindVertexArray(0);
}

void render_mesh(Mesh * mesh, mat4 * transform) {
    glUniformMatrix4fv(glGetUniformLocation(state.shader_default, "model"), 1, GL_FALSE, transform[0][0]);

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

void render_mesh_uv(Mesh * mesh, mat4 * transform, vec4 uv_rect) {
    glUniform4fv(glGetUniformLocation(state.shader_default, "uniform_UVs"), 1, uv_rect);
    render_mesh(mesh, transform);
}

uint32_t render_get_height() {
    return state.height;
}

uint32_t render_get_width() {
    return state.width;
}

