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

// TODO: Create a struct with all the inputs??
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

int main(int argc, char* argv[])
{  
    glfwInit();
    memory_init();
    
    // Window creation
    window_config w_conf = {
        .position_x = 0,
        .position_y = 0,
        .width = 1000,
        .height = 1000,
        .title = "game engine"};
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

    vec3 paddle_scale = {50, 200, 1};
    vec3 ball_scale = {25, 25, 1};
    mat4 left_paddle, right_paddle, ball;

    double ball_speed = 0.5;
    double ball_angle = M_PI/4;
    vec3 ball_velocity = {0.4 * cos(ball_angle), 0.4 * sin(ball_angle), 0};
    
    glm_mat4_identity(left_paddle);
    glm_mat4_identity(right_paddle);
    glm_mat4_identity(ball);

    vec3 translate_left_paddle = {30, 0, 0};
    vec3 translate_right_paddle = {render_get_width()-30, 0, 0};
    vec3 translate_ball = {render_get_width()*0.5, render_get_height()*0.5, 0};
    
    glm_translate(left_paddle, translate_left_paddle);
    glm_scale(left_paddle, paddle_scale);

    glm_translate(right_paddle, translate_right_paddle);
    glm_scale(right_paddle, paddle_scale);
    
    glm_translate(ball, translate_ball);
    glm_scale(ball, ball_scale);

    printf("%s", memory_usage_str());

    int left_score, right_score;
    left_score = right_score = 0;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

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
        translate_left_paddle[0] = translate_left_paddle[1] = 0;
        if (input_key_down(GLFW_KEY_W)) translate_left_paddle[1] += 0.1;
        if (input_key_down(GLFW_KEY_S)) translate_left_paddle[1] -= 0.1;

        glm_translate(left_paddle, translate_left_paddle);

        translate_right_paddle[0] = translate_right_paddle[1] = 0;
        if (input_key_down(GLFW_KEY_UP)) translate_right_paddle[1] += 0.1;
        if (input_key_down(GLFW_KEY_DOWN)) translate_right_paddle[1] -= 0.1;

        glm_translate(right_paddle, translate_right_paddle);
        
        // Keep the ball between the upper and lower bounds 
        //DEBUG("Ball X position: %f", ball[3][0]);
        //DEBUG("Ball Y position: %f", ball[3][1]);
        if (ball[3][1] < 12.5 || ball[3][1] > render_get_height()-12.5) {
            ball_velocity[1] = -ball_velocity[1]; 
        }

        // Collides with right paddle
        //DEBUG("Right paddle X position: %f", right_paddle[3][0]);
        //DEBUG("Right paddle Y position: %f", right_paddle[3][1]);
        if (ball[3][1] > right_paddle[3][1]-100 && ball[3][1] < right_paddle[3][1]+100) {
            if (ball[3][0] > render_get_width() - 67.5) {
                if (ball_velocity > 0) ball_velocity[0] = -ball_velocity[0];
                ball_velocity[1] += translate_right_paddle[1];
            }
        }

        // Collides with left paddle
        else if (ball[3][1] > left_paddle[3][1]-100 && ball[3][1] < left_paddle[3][1]+100) {
            if (ball[3][0] < 67.5) {
                if (ball_velocity[0] < 0) ball_velocity[0] = -ball_velocity[0];
                ball_velocity[1] += translate_left_paddle[1];
            }
        }

        if (ball[3][0] < 12.5) {
            INFO("RIGHT WINS!!");
            right_score++;
            INFO("Scores: %d : %d", left_score, right_score);
            ball[3][0] = render_get_width() * 0.5;
            ball[3][1] = render_get_height() * 0.5;
            ball_velocity[0] = 0.4 * cos(ball_angle);
            ball_velocity[1] = 0.4 * sin(ball_angle);
        } else if (ball[3][0] > render_get_width()-10) {
            INFO("LEFT WINS!!");
            left_score++;
            INFO("Scores: %d : %d", left_score, right_score);
            ball[3][0] = render_get_width() * 0.5;
            ball[3][1] = render_get_height() * 0.5;
            ball_velocity[0] = 0.4 * cos(ball_angle);
            ball_velocity[1] = 0.4 * sin(ball_angle);
        }
       
        glm_translate(ball, ball_velocity);

        // Rendering ====================================================== //
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        render_mesh(&square, &left_paddle); 
        render_mesh(&square, &right_paddle); 
        render_mesh(&square, &ball); 

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
