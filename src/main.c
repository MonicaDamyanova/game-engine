#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// TODO: Create a struct with all the inputs??
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

int main()
{   
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 800, "Game Engine", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Shaders ============================================================ //
   
    char * vertexShaderFile = "../src/default.vert"; // TODO: Make better file path?
    char * fragmentShaderFile = "../src/default.frag";

    char * buffer = 0;
    long length;
    FILE * f = fopen (vertexShaderFile, "rb");

    const char * vertexShaderSource = 0;
    const char * fragmentShaderSource = 0;

    // TODO: Create a function to handle shader loading.

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);
        buffer[length] = '\0';
    }

    if (buffer)
    {
        vertexShaderSource = buffer;   
    }
    
    f = fopen (fragmentShaderFile, "rb");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);
        buffer[length] = '\0';
    }

    if (buffer)
    {
        fragmentShaderSource = buffer;   
    }

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex data and attributes ========================================= //
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         //0.5f, -0.5f, 0.0f, // right 
         //0.0f,  0.5f, 0.0f  // top   
    }; 

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0);

    int vertexPositionLocation = glGetUniformLocation(shaderProgram, "bPos");

    // ==================================================================== //

    glfwSwapInterval(1); // Toggle for vsync

    struct timeval before, now;
    int frames = 0;
    float delta = 0;

    gettimeofday(&before, NULL);

    double xpos, ypos;

    glPointSize(10);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glfwGetCursorPos(window, &xpos, &ypos);

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

        printf("%f %f\n", xpos, ypos);

        // Update state =================================================== //
        glUniform2f(vertexPositionLocation, -1 + xpos/400, 1 -ypos/400);
        // TODO: Do not hardcode the dimensions of the window and do this
        // calculation in the shader.

        // Rendering ====================================================== //
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); 
        glDrawArrays(GL_POINTS, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}
