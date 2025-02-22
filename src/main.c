#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine/global.h"

// TODO: Create a struct with all the inputs??
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

int main(int argc, char* argv[])
{   
    render_init(); 
    
    glfwSwapInterval(1); // Toggle for vsync

    struct timeval before, now;
    int frames = 0;
    float delta = 0;

    gettimeofday(&before, NULL);

    while (!glfwWindowShouldClose(global.render.window))
    {
        processInput(global.render.window);

        // FPS tracking =================================================== //
        gettimeofday(&now, NULL); // glfwGetTime()??
        
        delta += (now.tv_sec - before.tv_sec) + (now.tv_usec - before.tv_usec) / 1000000.0; // us * 10^6 -> s
        before = now;
        frames++; 

        if (delta >= 1)
        {
            printf("FPS: %d\n", frames);
            delta--;
            frames = 0;
        }

        // Update state =================================================== //

        // Rendering ====================================================== //
        render_begin();

        render_quad(
                (vec2){global.render.width * 0.5, global.render.height * 0.5},
                (vec2){50, 50},
                (vec4){0, 0, 0, 1});

        render_end();
    }

    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}
