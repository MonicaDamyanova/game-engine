#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "../util.h"
#include "../global.h"

#include "render.h"
#include "render_internal.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    global.render.width = width;
    global.render.height = height;

    glm_ortho(0.0f, (float) width, 0.0f, (float) height, -2.0f, 2.0f, global.render.projection);

    glUniformMatrix4fv(
        glGetUniformLocation(global.render.shader_default, "projection"),
        1, GL_FALSE, &global.render.projection[0][0]
    );
}

GLFWwindow* render_init_window(uint32_t width, uint32_t height) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(global.render.width, global.render.height, "Game Engine", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        ERROR_EXIT("Failed to create GLFW window.\n");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        ERROR_EXIT("Failed to initialize GLAD.\n");
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetWindowAttrib(window, GLFW_FLOATING, GLFW_TRUE);

    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    return window;
}

void render_init_shaders(Render_State_Internal* state) {
    global.render.shader_default = render_shader_create("../shaders/default.vert", "../shaders/default.frag");

    glm_ortho(0.0f, (float) global.render.width, 0.0f, (float) global.render.height, -2.0f, 2.0f, global.render.projection);

    glUseProgram(global.render.shader_default);
    glUniformMatrix4fv(
        glGetUniformLocation(global.render.shader_default, "projection"),
        1, GL_FALSE, &global.render.projection[0][0]
    );
}

void render_init_color_texture(uint32_t* texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    uint8_t solid_white[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, solid_white);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void render_init_quad(uint32_t* vao, uint32_t* vbo, uint32_t* ebo) {
    //    x     y     z     u     v
    float vertices[] = {
        0.5,  0.5,  0.0,  0.0,  0.0,
        0.5, -0.5,  0.0,  0.0,  1.0,
       -0.5, -0.5,  0.0,  1.0,  1.0,
       -0.5,  0.5,  0.0,  1.0 , 0.0
    };

    uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glGenBuffers(1, ebo);

    glBindVertexArray(*vao);

    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // xyz
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


