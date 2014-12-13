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
#include <unistd.h>
#include "shader_utils.h"
#include "object.h"

//TODO finish 3d grid using instancing
enum Axis  { X_AXIS, Y_AXIS, Z_AXIS } ;
struct Player {
    glm::vec3 pos ;
    glm::vec3 vel ;
    glm::vec3 acc ;
    float h_angle ;
    float v_angle ;
    float fov ;
} ;
using std::vector ;

void draw_boid(Boid*) ;
void handle_input(GLFWwindow* window, Player* p, float dt, glm::mat4* view_ptr) ;
void render(vector<Boid> &boids, Vtx* vertices, int boid_s, double delta) ;
float* gen_2d_grid(int* size, int* num_pts, int dim, int step, Axis fixed_axis, float fixed_at) ;
float* gen_3d_grid(int* size, int* num_pts, int dim, int step) ;
float* gen_boid_normals(Vtx* vertices, int num_vertices, int* num_pts) ;
void debug_vtx(Vtx v);
void debug_vertices(Vtx* vs, int size);
double inline sec_to_ms(double seconds);

void update_physics(vector<Boid>& bs, vector<InvertedBox>& objs,
                    float x_max, float y_max, float z_max) ;


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
    int x_max = 100, y_max = 100, z_max = 100 ;
    int frames_rendered = 0 ;
    int physics_updates = 0 ;
    // Boid information
    int num_boids, boid_s, vert_s, vertices_s, elements_s ;
    vert_s = sizeof(Vtx) ; //for now just position and color info
    boid_s = 12 ; // 12 vertices per boid (4 triangles by 3 verteces)
    num_boids = 10 ; //for now
    vertices_s = num_boids * boid_s * vert_s ; //24
    vector<Boid> boids = generate_boids(10, 10, 10, num_boids) ;
    vector<InvertedBox> iboxes ;
    // ----------------------------- RESOURCES ----------------------------- //
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER) ; //Create, compile, link shader
    compile_shader("vertex_shader.glsl", vertex_shader) ;
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER) ;
    compile_shader("fragment_shader.glsl", fragment_shader) ;
    GLuint shader_prog = glCreateProgram() ;
    link_shader(shader_prog, {vertex_shader, fragment_shader}) ;
    glUseProgram(shader_prog) ;
    GLint pos_attr = glGetAttribLocation(shader_prog, "pos_model") ;
    GLint col_attr = glGetAttribLocation(shader_prog, "color_in") ;
    GLint nor_attr = glGetAttribLocation(shader_prog, "normal_model") ;

    // Create Vertex Array Objects
    GLuint vao_boids ; //boids
    glGenVertexArrays(1, &vao_boids) ;
    GLuint vao_grid ; //grid
    glGenVertexArrays(1, &vao_grid) ;
    GLuint vao_iboxes ;
    glGenVertexArrays(1, &vao_iboxes) ;
    //GLuint vao_normals ; //normals
    //glGenVertexArrays(1, &vao_normals) ;

    glBindVertexArray(vao_boids) ;
    GLuint vbo_boids ; // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &vbo_boids) ; //creates buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boids) ;
    int num_boid_vertices = boid_s * num_boids;
    Vtx vertices[num_boid_vertices] ;
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW) ;

    glEnableVertexAttribArray(pos_attr) ;
    glEnableVertexAttribArray(col_attr) ;
    glEnableVertexAttribArray(nor_attr) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boids) ;
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), 0) ;
    glVertexAttribPointer(col_attr, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)(3 * sizeof(GLfloat))) ;
    glVertexAttribPointer(nor_attr, 3, GL_FLOAT, GL_TRUE, sizeof(Vtx), (void*)(6 * sizeof(GLfloat))) ;

    glBindVertexArray(vao_grid) ;
    GLuint vbo_grid ; //grid vertex buffer object and vertex index object
    glGenBuffers(1, &vbo_grid) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_grid) ;
    int grid_size, num_grid_pts;
    float* grid_xyz = gen_2d_grid(&grid_size, &num_grid_pts, 10, 1, Y_AXIS, 0.0f) ;
    //float* grid_xyz = gen_3d_grid(&grid_size, &num_grid_pts, 10, 1) ;
    glBufferData(GL_ARRAY_BUFFER, grid_size, grid_xyz, GL_STATIC_DRAW) ;
    glEnableVertexAttribArray(pos_attr) ;
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0) ;
    glVertexAttrib3f(col_attr, 1.0f, 0.0f, 0.0f) ;

    glBindVertexArray(vao_iboxes) ;
    GLuint vbo_iboxes ;
    glGenBuffers(1, &vbo_iboxes) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_iboxes) ;
    int num_iboxes = 1 ;
    int ibox_pt_s = 3 ;
    int ibox_s = 24 ; //12 lines, 2 vertices per line
    int num_iboxes_pts = ibox_s * num_iboxes ;
    float ibox_vertices[num_iboxes_pts * ibox_pt_s] ;
    InvertedBox bounding_box (0,0,0,20, 20, 20,1,1,1,1) ;
    iboxes.push_back(bounding_box) ;
    InvertedBox::render(iboxes, ibox_vertices) ;
    glBufferData(GL_ARRAY_BUFFER, sizeof(ibox_vertices), ibox_vertices, GL_STATIC_DRAW) ;
    glEnableVertexAttribArray(pos_attr) ;
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0) ;
    glVertexAttrib3f(col_attr, 1.0f, 1.0f, 1.0f) ;

    //glBindVertexArray(vao_normals) ;
    //GLuint vbo_normals ;
    //glGenBuffers(1, &vbo_normals) ;
    //glBindBuffer(GL_ARRAY_BUFFER, vbo_normals) ;
    //int boid_normals_size ;
    //int num_normal_pts = 2 * num_boid_vertices ;
    //float* boid_normals = gen_boid_normals(vertices, num_boid_vertices, &boid_normals_size) ;
    //glBufferData(GL_ARRAY_BUFFER, boid_normals_size, boid_normals, GL_STATIC_DRAW) ;
    //glEnableVertexAttribArray(pos_attr) ;
    //glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0) ;
    //glVertexAttrib3f(col_attr, 1.0f, 1.0f, 1.0f) ;

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
    glm::vec3 white_light (1.0f,1.0f,1.0f) ;
    glm::vec3 ambient_light (0.1f, 0.1f, 0.1f) ;
    glUniform3fv(uni_amb, 1, &ambient_light[0]) ;
    GLint uni_light = glGetUniformLocation(shader_prog, "light_model") ;
    glm::vec3 light_pos (0.0f, 5.0f, 0.0f) ;
    glUniform3fv(uni_light, 1, &light_pos[0]) ;
    GLint uni_spec = glGetUniformLocation(shader_prog, "spec_coefficient") ;
    GLfloat specular_coefficient = 10.0f ;
    glUniform1f(uni_spec, specular_coefficient) ;

    glEnable(GL_DEPTH_TEST) ;

    double last_time = sec_to_ms(glfwGetTime()) ;
    double this_time, elapsed_time ;
    double lag_time = 0.0f ;
    double ms_per_update = 16.0f ;

    // ---------------------------- RENDERING ------------------------------ //
    //TODO update game loop to do fixed update with variable rendering
    while(!glfwWindowShouldClose(window)) {
        this_time = sec_to_ms(glfwGetTime()) ;
        elapsed_time = this_time - last_time;
        lag_time += elapsed_time ;
        last_time = this_time ;
       // std::cout << this_time << "\t" << last_time << "\t" << elapsed_time << "\t" << lag_time << "\n----------------\n" ;

        handle_input(window, &player1, 0.1f, &view) ;
        glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view)) ;
        glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f) ; // Clear the screen to black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

        while (lag_time >= ms_per_update) {
            //std::cout << "updating . . .\n" ;
            ++physics_updates ;
            update_physics(boids, iboxes, x_max, y_max, z_max) ;
            lag_time -= ms_per_update ;
        }

        ++frames_rendered ;

        glBindVertexArray(vao_boids) ;
        render(boids, vertices, num_boid_vertices, lag_time / ms_per_update) ; //TODO render with current velocity
        glBindBuffer(GL_ARRAY_BUFFER, vbo_boids) ;
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW) ;
        glDrawArrays(GL_TRIANGLES, 0, num_boid_vertices) ;


        glBindVertexArray(vao_grid) ;
        glVertexAttrib3f(col_attr, 1.0f, 0.0f, 0.0f) ;
        glUniform3fv(uni_amb, 1, &white_light[0]) ;
        glDrawArrays(GL_LINES, 0, num_grid_pts) ;

        glBindVertexArray(vao_iboxes) ;
        glVertexAttrib3f(col_attr, 1.0f, 1.0f, 1.0f) ;
        glDrawArrays(GL_LINES, 0, num_iboxes_pts) ;

        //glBindVertexArray(vao_normals) ;
        //glVertexAttrib3f(col_attr, 1.0f, 1.0f, 1.0f) ;
        //glDrawArrays(GL_LINES, 0, num_normal_pts) ;

        glfwSwapBuffers(window) ; // Swap buffers and poll window events
        glfwPollEvents() ;
        if (frames_rendered % 1000 == 0) {
            std::cout << "Frames rendered: " << frames_rendered << "\t physics updates: " << physics_updates << std::endl ;
        }
    }
    // ---------------------------- CLEARING ------------------------------ //
    free(grid_xyz) ;
    glDeleteProgram(shader_prog) ;
    glDeleteShader(fragment_shader) ;
    glDeleteShader(vertex_shader) ;
    glDeleteBuffers(1, &vbo_boids) ;
    glDeleteBuffers(1, &vbo_grid) ;
    //glDeleteBuffers(1, &vbo_normals) ;
    glDeleteVertexArrays(1, &vao_boids) ;
    glDeleteVertexArrays(1, &vao_grid) ;
    //glDeleteVertexArrays(1, &vao_normals) ;

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
    //std::cout << p->pos.z << "," << p->pos.y << "," << p->pos.z << "\n" ;
}
// TODO instancing
// TODO take into account time between frames (delta)
void render(vector<Boid> &boids, Vtx* vertices, int num_vs, double delta) {
    int s = (int) boids.size() ;
    Vtx vtx_0, vtx_1, vtx_2, vtx_3 ; //4 vertices per triangle
    Boid b ;
    V3 pos ;
    V3 t_0, t_1, t_2, v_0, v_1, v_2, v_3 ;
    V3 e_0, e_1,n ;
    int v_i = 0 ;
    for (int b_i=0; b_i < s; ++b_i) {
        b = boids[b_i] ;
        pos = b.pos ;
        t_0 = b.vel ;
        cross(&t_0, &b.acc, &t_1) ;
        cross(&t_1, &t_0, &t_2) ;
        normalize(&t_0) ;
        scale(2.0f, &t_0) ;
        add(&pos, &t_0, &v_0) ; //calculate bow vertex
        scale(-0.5f, &t_0) ; //calculate stern 3 vertices
        add(&pos, &t_0, &t_0) ;
        normalize(&t_2) ;
        add(&t_0, &t_2, &v_1) ; //up stern vertex
        normalize(&t_1) ;
        add(&t_0, &t_1, &v_2) ; //down stern vertex 1
        scale(-1.0f, &t_1) ;
        add(&t_0, &t_1, &v_3) ; //down stern vertex 2
        vtx_0.position = v_0 ;
        vtx_0.color = V3 {0.0f, 1.0f, 0.0f} ;
        sub(&v_1, &v_0, &e_0) ; //will reuse e_0 b/c shared by triangle 0,3,1
        sub(&v_2, &v_0, &e_1) ;
        cross(&e_0, &e_1, &n) ;
        vtx_0.normal = n ;
        vertices[v_i++] = vtx_0 ; //triangle 0,1,2
        vtx_1.position = v_1 ;
        vtx_1.color = V3 { 0.0f, 1.0f, 0.0f } ;
        vtx_1.normal = n ;
        vertices[v_i++] = vtx_1 ;
        vtx_2.position = v_2 ;
        vtx_2.color = V3 { 0.0f, 0.0f, 1.0f } ;
        vtx_2.normal = n ;
        vertices[v_i++] = vtx_2 ;
        sub(&v_3, &v_0, &e_1) ;//triangle 0,1,3
        cross(&e_1, &e_0, &n) ; //will reuse e_1 for triangle 0,2,3
        vtx_0.normal = n ; //vertex 0
        vertices[v_i++] = vtx_0 ;
        vtx_3.position = v_3 ;
        vtx_3.color = V3 {0.0f, 0.0f, 1.0f } ;
        vtx_3.normal = n ; //vertex 3
        vertices[v_i++] = vtx_3 ;
        vtx_1.normal = n ; //vertex 1
        vertices[v_i++] = vtx_1 ;
        sub(&v_2, &v_0, &e_0) ; //triangle 0,2,3
        cross(&e_0, &e_1, &n) ;
        vtx_0.normal = n ; //vertex 0
        vertices[v_i++] = vtx_0 ;
        vtx_2.normal = n ; //vertex 2
        vertices[v_i++] = vtx_2 ;
        vtx_3.normal = n ; //vertex 3
        vertices[v_i++] = vtx_3 ;
        sub(&v_3, &v_1, &e_0) ; //triangle 1,3,2
        sub(&v_2, &v_1, &e_1) ;
        cross(&e_0, &e_1, &n) ;
        vtx_1.normal = n ; //vertex 1
        vertices[v_i++] = vtx_1 ;
        vtx_3.normal = n ; //vertex 3
        vertices[v_i++] = vtx_3 ;
        vtx_2.normal = n ; //vertex 2
        vertices[v_i++] = vtx_2 ;
        //debug_vtx(&vtx_0) ;
        //debug_vtx(&vtx_1) ;
        //debug_vtx(&vtx_2) ;
        //debug_vtx(&vtx_3) ;
    } ;
}
void update_physics(vector<Boid>& bs, vector<InvertedBox>& iboxes,
                    float x_max, float y_max, float z_max) {
    update_flock(bs, iboxes, x_max, y_max, z_max) ;
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
float* gen_3d_grid(int* size, int* num_pts, int dim, int step) { //TODO gen only 1 dir then rotate 2x
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
void debug_vtx(Vtx v) {
    std::cout << "|p(" << v.position.x << "," << v.position.y << "," << v.position.z << ")" ;
    std::cout << "|c(" << v.color.x << "," << v.color.y << "," << v.color.z << ")" ;
    std::cout << "|n(" << v.normal.x << "," << v.normal.y << "," << v.normal.z << ")" ;
}
float* gen_boid_normals(Vtx *vertices, int num_vertices, int* size) {
    *size = num_vertices * 3 * 2 * (int) sizeof(float) ;
    float* boid_normals = (float*) malloc((size_t)*size) ;
    V3 p_0, p_1 ;
    V3 n ;
    int j = 0 ;
    for (int i=0; i<num_vertices; ++i) {
        p_0 = vertices[i].position ;
        n = vertices[i].normal ;
        normalize(&n) ;
        add(&n, &p_0, &p_1) ;
        boid_normals[j] = p_0.x ; ++j ;
        boid_normals[j] = p_0.y ; ++j ;
        boid_normals[j] = p_0.z ; ++j ;
        boid_normals[j] = p_1.x ; ++j ;
        boid_normals[j] = p_1.y ; ++j ;
        boid_normals[j] = p_1.z ; ++j ;
    }
    return boid_normals ;
}


void debug_vertices(Vtx* vs, int size) {
    std::cout << "Vertices|\n\t" ;
    for(int i=0; i<size; ++i) {
        debug_vtx(vs[i]) ;
    }
    std::cout << std::endl ;
}


double inline sec_to_ms(double seconds) {
    return seconds * 1000 ;
}
