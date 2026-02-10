#include "render.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdbool.h>
#include <stdio.h>

#include "logger.h"
#include "filesystem.h"

typedef struct shader {
    uint32_t program;

    int32_t cached_uniforms[TOTAL_CACHED_UNIFORMS];

    // add metadata for debug?
} Shader;

struct render_state {
    uint32_t width;
    uint32_t height;
 
    Shader shaders[MAX_SHADERS];
    uint32_t shader_count;
    ShaderHandle active_shader;
    
    mat4 projection;
};

static struct render_state state = {0};

uint32_t render_shader_create(const char* path_vert, const char* path_frag) {
    int success;
    char log[512];

    //INFO("Attempt reading %s.", path_vert);
    File file_vertex = io_file_read(path_vert);
    INFO("Completed io_file_read for %s successfully.", path_vert);
    if (!file_vertex.is_valid) {
        FATAL("Error reading shader: %s\n", path_vert);
        return 0;
    }
    //INFO("Successfully read %s.", path_vert);

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
    INFO("Completed io_file_read for %s successfully.", path_frag);
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

ShaderHandle render_shader_add(const char * path_vert, const char * path_frag) {
    uint32_t shader_program = render_shader_create(path_vert, path_frag);

    INFO("Shader program: %d", shader_program);
    
    if (shader_program == 0) {
        ERROR("Unable to add shader");
        return INVALID_SHADER;
    }

    Shader shader = {0};
    shader.program = shader_program;

    state.shaders[state.shader_count] = shader;
    return ++state.shader_count; // The handle will be index + 1
}

void render_shader_use(ShaderHandle handle) {
    if (state.active_shader != handle) { 
        glUseProgram(state.shaders[handle-1].program);
        state.active_shader = handle;
    }
    // Do config
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    struct render_state * state = glfwGetWindowUserPointer(window);

    glViewport(0, 0, width, height);
    state->width = width;
    state->height = height;

    glm_ortho(0.0f, (float) width, 0.0f, (float) height, -2.0f, 2.0f, state->projection);

    glUniformMatrix4fv(
        glGetUniformLocation(state->active_shader, "u_projection"),
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
    ShaderHandle default_shader = render_shader_add("../shaders/default.vert", "../shaders/default.frag");
    if (default_shader != DEFAULT_SHADER) {
        FATAL("Something went wrong with loading the default shader.");
    }

    glm_ortho(0.0f, (float) state.width, 0.0f, (float) state.height, -2.0f, 2.0f, state.projection);

    render_shader_use(DEFAULT_SHADER);
    render_shader_set_uniform_m4(state.active_shader, "u_projection", state.projection);

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

void render_mesh(Mesh * mesh, mat4 transform) {
    //glUniformMatrix4fv(glGetUniformLocation(state.shader_default, "model"), 1, GL_FALSE, transform[0][0]);
    render_shader_set_uniform_m4(state.active_shader, "u_projection", state.projection);
    render_shader_set_uniform_m4(state.active_shader, "u_model", transform);

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

void render_mesh_uv(Mesh * mesh, mat4 transform, vec4 uv_rect) {
    //glUniform4fv(glGetUniformLocation(state.shader_default, "uniform_UVs"), 1, uv_rect);
    render_shader_set_uniform_v4(state.active_shader, "u_UVs", uv_rect);
    render_mesh(mesh, transform);
}

// UNIFORMS ========================================================================= //

int32_t shader_uniform_validation(ShaderHandle handle, const char * name) {
    //GLint current_program;
    //glGetIntegerv(GL_CURRENT_PROGRAM, &current_program); 
    //if (state.shaders[state.active_shader-1].program != current_program) {

    if (handle != state.active_shader) {
        ERROR("The requested shader is not bound!");
        return -1;
    }
    int32_t loc = glGetUniformLocation(state.shaders[handle-1].program, name);
    if (loc == -1) {
        ERROR("Invalid uniform with name '%s' in program %d.", name, state.shaders[handle-1].program);
        return -1;
    }
    return loc;
}

void render_shader_set_uniform_i(ShaderHandle handle, const char * name, int value) {
    int32_t loc = shader_uniform_validation(handle, name);
    if (loc == -1) return;
    glUniform1i(loc, value);
}

void render_shader_set_uniform_f(ShaderHandle handle, const char * name, float value);
void render_shader_set_uniform_v2(ShaderHandle handle, const char * name, vec2 value);
void render_shader_set_uniform_v3(ShaderHandle handle, const char * name, vec3 value);

void render_shader_set_uniform_v4(ShaderHandle handle, const char * name, vec4 value) {
    int32_t loc = shader_uniform_validation(handle, name);
    if (loc == -1) return;
    glUniform4fv(loc, 1, value);
}

void render_shader_set_uniform_m3(ShaderHandle handle, const char * name, mat3 value);

void render_shader_set_uniform_m4(ShaderHandle handle, const char * name, mat4 value) {
    int32_t loc = shader_uniform_validation(handle, name);
    if (loc == -1) return;
    glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
    return;
}

// UNIFORMS END ===================================================================== //

//void render_use_default_shader() {
//    glUseProgram(state.shaders[DEFAULT_SHADER].program);
    //glUniform1i(glGetUniformLocation(state.shader_default, "textureID"), 0);
    //glDisable(GL_BLEND);
//    glEnable(GL_DEPTH_TEST);
//}

uint32_t render_get_height() {
    return state.height;
}

uint32_t render_get_width() {
    return state.width;
}

