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

int main(int argc, char* argv[]) {  
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

    render_upload_mesh(&square, vertices, indices, sizeof(vertices), sizeof(indices));

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
