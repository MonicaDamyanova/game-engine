#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "window.h"
#include "logger.h"
#include "memory.h"
#include "render.h"
#include "input.h"
#include "transform.h"

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

window_config w_conf = {
    .position_x = 0,
    .position_y = 0,
    .width = 1000,
    .height = 1000,
    .title = "game engine"
};

Mesh square = {0};

void init_square(Mesh * mesh) {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

int main(int argc, char* argv[])
{  
    glfwInit();
    memory_init();
    
    // Window creation
    GLFWwindow * window = window_init(&w_conf);
    if (!window) return -1;

    // Render initialization
    render_init(window); 
    input_init(window); 
    
    glfwSwapInterval(1); // Toggle for vsync

    // Game states 
    struct timeval before, now;
    int frames = 0;
    float delta = 0;
    gettimeofday(&before, NULL);

    init_square(&square);

    INFO("Initialized Square");

    Transform2D transform = {
        .position = {500, 500},
        .scale = {100, 100},
        .rotation = 0.0
    };

    mat4 matrix;

    printf("%s", memory_usage_str());

    while (!glfwWindowShouldClose(window))
    {
        if (input_key_down(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, 1);

        // FPS tracking =================================================== //
        gettimeofday(&now, NULL); // glfwGetTime()??
        
        delta += (now.tv_sec - before.tv_sec) + (now.tv_usec - before.tv_usec) / 1000000.0; // us * 10^6 -> s
        before = now;
        frames++; 

        if (delta >= 1) {
            printf("FPS: %d\n", frames);
            delta--;
            frames = 0;
            printf("%s", memory_usage_str());
        }

        // Update state =================================================== //
        transform.rotation += 0.01;

        transform_matrix(&transform, matrix);

        // Rendering ====================================================== //
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        render_mesh(&square, &matrix); 
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteProgram(shaderProgram);

    glfwTerminate();
    memory_shutdown();

    return 0;
}
