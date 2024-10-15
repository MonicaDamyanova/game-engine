#include <stdio.h>
#include <sys/time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main() {
    
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 800, "GOODNIGHT GAYMER", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glfwSwapInterval(1); // Toggle for vsync

    glViewport(0, 0, 800, 800);

    struct timeval before, now;
    int frames = 0;
    float delta = 0;

    gettimeofday(&before, NULL);

    while (!glfwWindowShouldClose(window)) {

        // FPS tracking =================================================== //
        gettimeofday(&now, NULL);
        
        delta += (now.tv_sec - before.tv_sec) + (now.tv_usec - before.tv_usec) / 1000000.0; // us * 10^6 -> s
        before = now;
        frames++; 

        if (delta >= 1) {
            printf("FPS: %d\n", frames);
            delta--;
            frames = 0;
        }

        // Rendering ====================================================== //
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
