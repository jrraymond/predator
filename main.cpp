#include <iostream>
#include <limits>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "boids.h"

#define GLSL(src) "#version 150 core\n" #src

std::vector<Boid> generate_boids(int max_x, int max_y, int max_z, int number) ;
void render(std::vector<Boid>) ;
void draw_boid(Boid*) ;


int main() {
    // -------------------------------- INIT ------------------------------- //
    if (glfwInit() != GL_TRUE) {
        fprintf(stderr, "Failed to initialize GLFW\n") ;
        return -1 ;
    }
    // Create a rendering window with OpenGL 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3) ;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2) ;
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE) ;
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE) ;
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE) ;

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL) ;
    glfwMakeContextCurrent(window) ;

    glewExperimental = GL_TRUE ;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n") ;
        return -1 ;
    }
    // ----------------------------- RESOURCES ----------------------------- //

    // Create Vertex Array Object
    GLuint vao ;
    glGenVertexArrays(1, &vao) ;
    glBindVertexArray(vao) ;

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo ;
    glGenBuffers(1, &vbo) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo) ;

    float vertices[] = {
            0.0f,  0.5f, // Vertex 1 (X, Y)
            0.5f, -0.5f, // Vertex 2 (X, Y)
            -0.5f, -0.5f  // Vertex 3 (X, Y)
    } ;

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW) ;

    // Create and compile the vertex shader
    const char* vertex_src = GLSL(
    in vec2 position ;

    void main() {
        gl_Position = vec4(position, 0.0, 1.0) ;
    }
    ) ;

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER) ;
    glShaderSource(vertex_shader, 1, &vertex_src, NULL) ;
    glCompileShader(vertex_shader) ;

    // Create and compile the fragment shader
    const char* fragment_src = GLSL(
        out vec4 outColor ;

        void main() {
            outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f) ;
        }
    ) ;

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER) ;
    glShaderSource(fragment_shader, 1, &fragment_src, NULL) ;
    glCompileShader(fragment_shader) ;

    // Link the vertex and fragment shader into a shader program
    GLuint shader_prog = glCreateProgram() ;
    glAttachShader(shader_prog, vertex_shader) ;
    glAttachShader(shader_prog, fragment_shader) ;
    glBindFragDataLocation(shader_prog, 0, "outColor") ;
    glLinkProgram(shader_prog) ;
    glUseProgram(shader_prog) ;

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shader_prog, "position") ;
    glEnableVertexAttribArray(posAttrib) ;
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0) ;

    // ---------------------------- RENDERING ------------------------------ //
    while(!glfwWindowShouldClose(window))
    {
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f) ;
        glClear(GL_COLOR_BUFFER_BIT) ;

        // Draw a triangle from the 3 vertices
        glDrawArrays(GL_TRIANGLES, 0, 3) ;

        // Swap buffers and poll window events
        glfwSwapBuffers(window) ;
        glfwPollEvents() ;
    }

    // ---------------------------- CLEARING ------------------------------ //
    glDeleteProgram(shader_prog) ;
    glDeleteShader(fragment_shader) ;
    glDeleteShader(vertex_shader) ;
    glDeleteBuffers(1, &vbo) ;
    glDeleteVertexArrays(1, &vao) ;

    // ---------------------------- TERMINATE ----------------------------- //
    glfwTerminate() ;

    return EXIT_SUCCESS ;
}

std::vector<Boid> generate_boids(int max_x, int max_y, int max_z, int number) {
    std::vector<Boid> boids ;
    boids.reserve(number) ;
    std::random_device                  rand_dev ;
    std::mt19937                        generator(rand_dev()) ;
    std::uniform_int_distribution<int>  r_x(0, max_x) ;
    std::uniform_int_distribution<int>  r_y(0, max_y) ;
    std::uniform_int_distribution<int>  r_z(0, max_z) ;
    int x, y, z ;
    for (int i = 0 ; i < number ; ++i) {
        x = r_x(generator) ;
        y = r_y(generator) ;
        z = r_z(generator) ;
        V3 p = V3{(float) x, (float) y, (float) z} ;
        V3 v = V3{(float) rand() / RAND_MAX, (float) rand() / RAND_MAX, (float) rand() / RAND_MAX} ;
        V3 a = V3{(float) rand() / RAND_MAX, (float) rand() / RAND_MAX, (float) rand() / RAND_MAX} ;
        boids.push_back(Boid {p, v, a}) ;
    }
    return boids ;
}

void render(std::vector<Boid> boids) {
}
void draw_boid(Boid* boid) {
}
