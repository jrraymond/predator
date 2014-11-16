#include <iostream>
#include <limits>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include "boids.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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

    GLFWwindow* window = glfwCreateWindow(600, 600, "OpenGL", NULL, NULL) ;
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
            0.5f, 0.5f, 1.0f, 1.0f, 1.0f, //top-right
            -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, //top-left
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, //bottom-right
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f //bottom-left
    } ;
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW) ;

    // Create an element array
    GLuint ebo ;
    glGenBuffers(1, &ebo) ;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo) ;
    GLuint elements[] = {
            0, 1, 2,
            1, 3, 2
    } ;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW) ;

    // Create and compile the vertex shader
    const char* vertex_src = GLSL(
        in vec2 position ;
        in vec3 color ;
        out vec3 Color ;
        uniform mat4 trans ;
        void main() {
            Color = color ;
            gl_Position = trans * vec4(position, 0.0, 1.0) ;
        }
    ) ;
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER) ;
    glShaderSource(vertex_shader, 1, &vertex_src, NULL) ;
    glCompileShader(vertex_shader) ;

    // Create and compile the fragment shader
    const char* fragment_src = GLSL(
        uniform vec3 vertex_color ;
        in vec3 Color ;
        out vec4 out_color ;

        void main() {
            out_color = vec4(vertex_color, 1.0f) ;
        }
    ) ;
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER) ;
    glShaderSource(fragment_shader, 1, &fragment_src, NULL) ;
    glCompileShader(fragment_shader) ;


    // Link the vertex and fragment shader into a shader program
    GLuint shader_prog = glCreateProgram() ;
    glAttachShader(shader_prog, vertex_shader) ;
    glAttachShader(shader_prog, fragment_shader) ;
    glBindFragDataLocation(shader_prog, 0, "out_color") ;
    glLinkProgram(shader_prog) ;
    glUseProgram(shader_prog) ;

    // Specify the layout of the vertex data
    GLint pos_attr = glGetAttribLocation(shader_prog, "position") ;
    glEnableVertexAttribArray(pos_attr) ;
    glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0) ;
    GLint col_attr = glGetAttribLocation(shader_prog, "color") ;
    glEnableVertexAttribArray(col_attr) ;
    glVertexAttribPointer(col_attr, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat))) ;


    GLint uni_trans = glGetUniformLocation(shader_prog, "trans") ;


    // ---------------------------- RENDERING ------------------------------ //
    while(!glfwWindowShouldClose(window))
    {
        GLint uni_color = glGetUniformLocation(shader_prog, "vertex_color") ;
        float time = (float) glfwGetTime() ;
        float adj_factor = sin(time) - 0.5f ;
        glUniform3f(uni_color, adj_factor, adj_factor, adj_factor) ;

        glm::mat4 trans; //simple 2d rotation
        trans = glm::rotate(trans, (float)clock()/(float) CLOCKS_PER_SEC*180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uni_trans, 1, GL_FALSE, glm::value_ptr(trans));

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f) ;
        glClear(GL_COLOR_BUFFER_BIT) ;

        // Draw a triangle from the 3 vertices glDrawArrays(GL_TRIANGLES, 0, 3) ;
        // Draw using element buffer
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0) ;

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
