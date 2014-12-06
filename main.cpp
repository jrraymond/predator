#include <iostream>
#include <fstream>
#include <limits>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include "boids.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#define MAX_SHADER_SRC_SIZE 4096
//TODO add lighting
//TODO draw grid
enum Axis  { X_AXIS, Y_AXIS, Z_AXIS } ;
struct Player {
    glm::vec3 pos ;
    glm::vec3 vel ;
    glm::vec3 acc ;
    float h_angle ;
    float v_angle ;
    float fov ;
} ;
std::vector<Boid> generate_boids(int max_x, int max_y, int max_z, int number) ;
void draw_boid(Boid*) ;
void handle_input(GLFWwindow* window, Player* p, float dt, glm::mat4* view_ptr) ;
void render(std::vector<Boid>, float* vertices, int boid_s) ;
bool shader_is_valid(GLuint sp);
void print_shader_link_info(GLuint sp);
void print_shader_comp_info(GLuint shader_index);
bool read_file(const char* f_name, char* str, int max_len);
bool compile_shader(const char* f_name, GLuint shader) ;
bool link_shader(GLuint prog, std::initializer_list<GLuint> shaders) ;
float* gen_2d_grid(int* size, int* num_pts, int dim, int step, Axis fixed_axis, float fixed_at) ;
float* gen_3d_grid(int* size, int* num_pts, int dim, int step) ;


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
    Player player1 = Player { glm::vec3(10.0f, 10.0f, 10.0f)
                            , glm::vec3(0.0f, 0.0f, 0.0f)
                            , glm::vec3(0.0f, 0.0f, 0.0f)
                            , 4.0f ,-0.6f, 45.0f } ;
    int params = -1 ;
    // Boid information
    int boids_s, boid_s, vert_s, vertices_s, elements_s ;
    vert_s = 6 ; //for now just position and color info
    boid_s = 4 ; // four vertices per boid
    boids_s = 1 ; //for now
    elements_s = boids_s * boid_s * 3 ; //12
    vertices_s = boids_s * boid_s * vert_s ; //24
    std::vector<Boid> boids = generate_boids(10, 10, 10, boids_s) ; //TODO use a C array instead because its more metal
    debug_boid(&boids[0]) ;
    // ----------------------------- RESOURCES ----------------------------- //
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER) ; //Create, compile, link shader
    compile_shader("vertex_shader.glsl", vertex_shader) ;
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER) ;
    compile_shader("fragment_shader.glsl", fragment_shader) ;
    GLuint shader_prog = glCreateProgram() ;
    link_shader(shader_prog, {vertex_shader, fragment_shader}) ;
    glUseProgram(shader_prog) ;
    GLint pos_attr = glGetAttribLocation(shader_prog, "position") ;
    GLint col_attr = glGetAttribLocation(shader_prog, "color_in") ;

    // Create Vertex Array Object
    GLuint vao_boids ; //boids
    glGenVertexArrays(1, &vao_boids) ;

    GLuint vao_grid ; //grid
    glGenVertexArrays(1, &vao_grid) ;

    glBindVertexArray(vao_boids) ;
    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo_boids ;
    glGenBuffers(1, &vbo_boids) ; //creates buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boids) ;
    //float vertices[vertices_s] ;
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW) ;
    float vertices[4*6] ;
    render(boids, vertices, 4*6) ;
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW) ;

    // Create an element array
    GLuint ebo ;
    glGenBuffers(1, &ebo) ;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo) ;
    //GLuint elements[elements_s] ;
    //for (GLuint i=0; i < boids_s; ++i) {
    //    elements[i] = i ; elements[i+1] = i + 1 ; elements[i+2] = i + 2 ; //side v0,v1,v2
    //    elements[i+3] = i ; elements[i+4] = i + 2 ; elements[i+5] = i + 3 ; //side v0, v3,v4
    //    elements[i+6] = i ; elements[i+7] = i + 3 ; elements[i+8] = i + 1 ; //side v0,v4,v2
    //    elements[i+9] = i + 1 ; elements[i+10] = i + 3 ; elements[i+11] = i + 2 ; //side v1,v3,v2
    //}
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW) ;
    GLuint elements[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 1,
        1, 3, 2
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW) ;
    glEnableVertexAttribArray(pos_attr) ;
    glEnableVertexAttribArray(col_attr) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boids) ;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo) ;
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0) ;
    glVertexAttribPointer(col_attr, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))) ;

    glBindVertexArray(vao_grid) ;
    GLuint vbo_grid ; //grid vertex buffer object and vertex index object
    glGenBuffers(1, &vbo_grid) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_grid) ;
    int grid_size, grid_num_pts ;
    float* grid_xyz = gen_2d_grid(&grid_size, &grid_num_pts, 10, 1, Y_AXIS, 0.0f) ;
    //float* grid_xyz = gen_3d_grid(&grid_size, &grid_num_pts, 100, 10) ;
    glBufferData(GL_ARRAY_BUFFER, grid_size, grid_xyz, GL_STATIC_DRAW) ;

    glEnableVertexAttribArray(pos_attr) ;
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0) ;
    glVertexAttrib3f(col_attr, 1.0f, 0.0f, 0.0f) ;

    glm::mat4 model = glm::mat4() ;
    glm::mat4 view = glm::lookAt(
            glm::vec3(10.0f, 10.0f, 10.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
    ) ;
    glm::mat4 proj = (glm::mat4) glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

    GLint uni_model = glGetUniformLocation(shader_prog, "model") ;
    GLint uni_view = glGetUniformLocation(shader_prog, "view") ;
    GLint uni_proj = glGetUniformLocation(shader_prog, "proj") ;
    glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uni_proj, 1, GL_FALSE, glm::value_ptr(proj));
    GLint uni_amb = glGetUniformLocation(shader_prog, "ambient") ;
    glm::vec3 ambient_light (0.5f, 0.5f, 0.5f) ;
    glUniform3fv(uni_amb, 1, &ambient_light[0]) ;

    glEnable(GL_DEPTH_TEST) ;

    // ------------------------ Handling Input ----------------------------- //
    float lastTime = glfwGetTime() ;
    float thisTime, deltaTime ;
   // for (int i=0; i<vertices_s; ++i) { vertices[i] = vertices[i] * 0.2 ; }
    //glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_s, &vbo) ;

    // ---------------------------- RENDERING ------------------------------ //
    while(!glfwWindowShouldClose(window)) {
        lastTime = thisTime ;
        thisTime = glfwGetTime() ;
        deltaTime = thisTime - lastTime ;
        //GLint uni_color = glGetUniformLocation(shader_prog, "vertex_color") ;
        //float time = (float) glfwGetTime() ;
        //float adj_factor = sin(time) - 0.5f ;
        //glUniform3f(uni_color, adj_factor, adj_factor, adj_factor) ;

        handle_input(window, &player1, deltaTime, &view) ;
        glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view)) ;
        glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f) ; // Clear the screen to black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

        glBindVertexArray(vao_boids) ;
        //render(boids, vertices, boid_s * vert_s) ;
        glDrawElements(GL_TRIANGLES, elements_s, GL_UNSIGNED_INT, 0) ; // Draw using element buffer

        glBindVertexArray(vao_grid) ;
        glDrawArrays(GL_LINES, 0, grid_num_pts) ;

        glfwSwapBuffers(window) ; // Swap buffers and poll window events
        glfwPollEvents() ;
    }
    // ---------------------------- CLEARING ------------------------------ //
    free(grid_xyz) ;
    glDeleteProgram(shader_prog) ;
    glDeleteShader(fragment_shader) ;
    glDeleteShader(vertex_shader) ;
    glDeleteBuffers(1, &vbo_boids) ;
    glDeleteBuffers(1, &vbo_grid) ;
    glDeleteVertexArrays(1, &vao_boids) ;
    glDeleteVertexArrays(1, &vao_grid) ;

    // ---------------------------- TERMINATE ----------------------------- //
    glfwTerminate() ;

    return EXIT_SUCCESS ;
}

void handle_input(GLFWwindow* window, Player* p, float dt, glm::mat4* view_ptr) {
    float speed = 10 ;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { p->h_angle -= 0.1f ; }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { p->h_angle += 0.1f ; }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { p->v_angle -= 0.1f ; }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { p->v_angle += 0.1f ; }
    glm::vec3 dir = glm::vec3( cos(p->v_angle) * sin(p->h_angle)
                             , sin(p->v_angle)
                             , cos(p->v_angle) * cos(p->h_angle)) ;
    glm::vec3 right = glm::vec3( sin(p->h_angle - 3.14f/2.0f)
                               , 0
                               , cos(p->h_angle - 3.14f/2.0f)) ;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { p->pos += right * dt * speed ; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { p->pos -= right * dt * speed ;  }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { p->pos -= dir * dt * speed ; }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { p->pos += dir * dt * speed ; }
    glm::vec3 up = glm::cross(right, dir) ;
    *view_ptr = glm::lookAt(p->pos, p->pos + dir, up) ;
}

void render(std::vector<Boid> boids, float* vertices, int num_vs) {
    int s = (int) boids.size() ;
    Boid b ;
    V3 pos ;
    V3 t_0, t_1, t_2, v_0, v_1, v_2, v_3 ;
    int v_i = 0 ;
    for (int b_i=0; b_i < s; ++b_i) {
        b = boids[b_i] ;
        pos = b.pos ;
        t_0 = b.vel ;
        cross(&t_0, &b.acc, &t_1) ;
        cross(&t_0, &t_1, &t_2) ;
        add(&pos, &t_0, &v_0) ; //calculate bow vertex
        scale(-1.0f, &t_0) ; //calculate stern 3 vertices
        add(&t_0, &t_1, &v_1) ; //up stern vertex
        add(&t_0, &t_2, &v_2) ; //down stern vertex 1
        scale(-1.0f, &t_2) ;
        add(&t_0, &t_2, &v_3) ; //down stern vertex 2
        normalize(&v_0) ;
        normalize(&v_1) ;
        normalize(&v_2) ;
        normalize(&v_3) ;
        vertices[v_i] = v_0.x ;
        vertices[v_i+1] = v_0.y ;
        vertices[v_i+2] = v_0.z ;
        vertices[v_i+3] = 1.0f ; vertices[v_i+4] = 1.0f ; vertices[v_i+5] = 1.0f ;
        vertices[v_i+6] = v_1.x ;
        vertices[v_i+7] = v_1.y ;
        vertices[v_i+8] = v_1.z ;
        vertices[v_i+9] = 1.0f ; vertices[v_i+10] = 1.0f ; vertices[v_i+11] = 1.0f ;
        vertices[v_i+12] = v_2.x ;
        vertices[v_i+13] = v_2.y ;
        vertices[v_i+14] = v_2.z ;
        vertices[v_i+15] = 1.0f ; vertices[v_i+16] = 1.0f ; vertices[v_i+17] = 1.0f ;
        vertices[v_i+18] = v_3.x ;
        vertices[v_i+19] = v_3.y ;
        vertices[v_i+20] = v_3.z ;
        vertices[v_i+21] = 1.0f ; vertices[v_i+22] = 1.0f ; vertices[v_i+23] = 1.0f ; v_i += num_vs ;
        debug_v3(&v_0, "v_0") ;
        debug_v3(&v_1, "v_1") ;
        debug_v3(&v_2, "v_2") ;
        debug_v3(&v_3, "v_3") ;
    }
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
    V3 p, v, a ;
    for (int i = 0 ; i < number ; ++i) {
        x = r_x(generator) ;
        y = r_y(generator) ;
        z = r_z(generator) ;
        p = V3{(float) x, (float) y, (float) z} ;
        v = V3{(float) rand() / RAND_MAX, (float) rand() / RAND_MAX, (float) rand() / RAND_MAX} ;
        a = V3{(float) rand() / RAND_MAX, (float) rand() / RAND_MAX, (float) rand() / RAND_MAX} ;
        boids.push_back(Boid {p, v, a}) ;
    }
    return boids ;
}


void print_shader_comp_info(GLuint shader_index) {
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetShaderInfoLog (shader_index, max_length, &actual_length, log);
    printf ("shader info log for GL index %i:\n%s\n", shader_index, log);
}
void print_shader_link_info(GLuint sp) {
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetProgramInfoLog (sp, max_length, &actual_length, log);
    printf ("program info log for GL index %i:\n%s", sp, log);
}
bool shader_is_valid(GLuint sp) {
    int params = -1;
    glValidateProgram (sp);
    glGetProgramiv (sp, GL_VALIDATE_STATUS, &params);
    printf ("program %i GL_VALIDATE_STATUS = %i\n", sp, params);
    if (GL_TRUE != params) {
        print_shader_link_info(sp);
        return false;
    }
    return true;
}


bool read_file(const char* f_name, char* str, int max_len) {
    FILE* fp = fopen(f_name , "r") ;
    int len = 0 ;
    char line[80] ;

    str[0] = '\0' ;
    if (fp == NULL) {
        fprintf(stderr, "ERROR: error opening file: %s\n", f_name) ;
        return false ;
    }
    strcpy(line, "") ;
    while (!feof(fp)) {
        if (NULL != fgets(line, 80, fp)) {
            len += strlen(line) ;
            if (len >= max_len) {
                fprintf(stderr, "ERROR: file length greater than max length: %i", max_len ) ;
            }
            strcat(str, line) ;
        }
    }
    if (fclose(fp) == EOF) {
        fprintf(stderr, "ERROR: closing file %s\n", f_name) ;
        return false ;
    }
    return true ;
}
/* compiles a shader given file name containing source and shader index */
bool compile_shader(const char* f_name, GLuint shader) {
    GLint status ;
    char src[MAX_SHADER_SRC_SIZE] ;
    if (!read_file(f_name, src, MAX_SHADER_SRC_SIZE)) {  return false ;  }
    const GLchar* glsrc = (const GLchar*) src ;
    glShaderSource(shader, 1, &glsrc, NULL) ;
    glCompileShader(shader) ;
    //check for compilation errors
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status) ;
    if (GL_TRUE != status) {
        fprintf(stderr, "ERROR: GL shader %s index %i did not compile\n", f_name, shader) ;
        print_shader_comp_info(shader) ;
        return false ;
    }
    return true ;
}
bool link_shader(GLuint prog, std::initializer_list<GLuint> shaders) {
    GLint status ;
    for(auto x : shaders) {
        glAttachShader(prog, x) ;
    }
    glLinkProgram(prog) ;
    glGetProgramiv(prog, GL_LINK_STATUS, &status) ;
    if (GL_TRUE != status) {
        fprintf(
                stderr,
                "ERROR: could not link shader programme GL index %i\n",
                prog
        );
        print_shader_link_info(prog) ;
        return false ;
    }
    return true ;
}


float* gen_2d_grid(int* size, int* num_pts, int dim, int step, Axis fixed_axis, float fixed_at) {
    int vert_s = 3 ;
    *num_pts = (2 * dim / step + 1) * 4 ;
    *size = *num_pts * vert_s * (int) sizeof(float) ;
    float* pts = (float*) malloc((size_t)*size) ;
    int i = 0 ;
    switch (fixed_axis) {
        case X_AXIS :
            for (int y = -dim; y <= dim; y += step) {
                pts[i++] = fixed_at ;
                pts[i++] = (float) y ;
                pts[i++] = (float) -dim ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) y ; //repeat for grid max
                pts[i++] = (float) dim ;
            }
            for (int z = -dim; z <= dim; z += step) { //now fix x
                pts[i++] = fixed_at ;
                pts[i++] = (float) -dim ;
                pts[i++] = (float) z ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) dim ;
                pts[i++] = (float) z ;
            }
            break ;
        case Y_AXIS:
            for (int x = -dim; x <= dim; x += step) {
                pts[i++] = (float) x ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) -dim ;
                pts[i++] = (float) x ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) dim ;
            }
            for (int z = -dim; z <= dim; z += step) {
                pts[i++] = (float) -dim ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) z ;
                pts[i++] = (float) dim ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) z ;
            }
            break ;
        case Z_AXIS:
            for (int x = -dim; x <= dim; x += step) {
                pts[i++] = (float) x ;
                pts[i++] = (float) -dim ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) x ;
                pts[i++] = (float) dim ;
                pts[i++] = fixed_at ;
            }
            for (int y = -dim; y <= dim; y += step) { //now fix x
                pts[i++] = (float) -dim ;
                pts[i++] = (float) y ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) dim ;
                pts[i++] = (float) y ;
                pts[i++] = fixed_at ;
            }
            break ;
    }
    return pts ;
}
float* gen_3d_grid(int* size, int* num_pts, int dim, int step) {
    int vert_s = 3 ;
    *num_pts = 5292 ;//(2 * dim / step + 1) * 4 ;
    *size = *num_pts * vert_s * (int) sizeof(float) ;
    float* pts = (float*) malloc((size_t)*size) ;
    int i = 0 ;
    for (int x = -dim; x <= dim; x += step) {
        for (int y = -dim; y <= dim; y += step) {
            pts[i++] = (float) x;
            pts[i++] = (float) y;
            pts[i++] = (float) -dim;
            pts[i++] = (float) x;
            pts[i++] = (float) y; //repeat for grid max
            pts[i++] = (float) dim;
        }
        for (int z = -dim; z <= dim; z += step) { //now fix x
            pts[i++] = (float) x;
            pts[i++] = (float) -dim;
            pts[i++] = (float) z;
            pts[i++] = (float) x;
            pts[i++] = (float) dim;
            pts[i++] = (float) z;
        }
    }
    return pts ;
}
