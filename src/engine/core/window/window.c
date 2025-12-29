#include "window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logger.h"

//static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    glViewport(0, 0, width, height);
//   global.render.width = width;
//    global.render.height = height;
//
//    glm_ortho(0.0f, (float) width, 0.0f, (float) height, -2.0f, 2.0f, global.render.projection);
//
//    glUniformMatrix4fv(
//        glGetUniformLocation(global.render.shader_default, "projection"),
//        1, GL_FALSE, &global.render.projection[0][0]
//    );
//}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    WARN("No frame buffer size callback implemented.");
    // Add some logic to adjust the render based on the new width and height.
}

GLFWwindow * window_init(window_config * config) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(config->width, config->height, config->title, NULL, NULL);
    
    if (window == NULL) {
        FATAL("Failed to create GLFW window.");
        return NULL;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Probably worth creating a window option struct.
    // And pass it into this function as a parameter.
    glfwSetWindowAttrib(window, GLFW_FLOATING, GLFW_TRUE);

    return window;
}

void window_destroy(GLFWwindow * window) {
    if (!window) return;
    glfwDestroyWindow(window);
}
