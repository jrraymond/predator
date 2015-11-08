//
// Created by justin on 11/7/15.
//

#include "GraphicsEngine.h"
#include "../lib/shader_utils/shader_utils.h"
#include "../object.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

void render_old(const vector<PhysicsObject> &boids, Vtx *vertices);

bool GraphicsEngine::initialized = false;

GraphicsEngine::GraphicsEngine(GameState state) {
    GraphicsEngine::initialized = true;
    const char* vtx_f_name = "vertex_shader.glsl";
    const char* frag_f_name = "fragment_shader.glsl";
    vertex_shader = glCreateShader(GL_VERTEX_SHADER) ; //Create, compile, link shader
    compile_shader(vtx_f_name, vertex_shader) ;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER) ;
    compile_shader(frag_f_name, fragment_shader) ;
    GLuint shader_prog = glCreateProgram() ;
    link_shader(shader_prog, {vertex_shader, fragment_shader}) ;
    shader_program = shader_prog;
    glUseProgram(shader_program) ;

    pos_attr = glGetAttribLocation(shader_prog, "pos_model") ;
    col_attr = glGetAttribLocation(shader_prog, "color_in") ;
    nor_attr = glGetAttribLocation(shader_prog, "normal_model") ;

    // Create Vertex Array Objects
    glGenVertexArrays(1, &vao_boids) ;
    glGenVertexArrays(1, &vao_grid) ;
    glGenVertexArrays(1, &vao_iboxes) ;

    glBindVertexArray(vao_boids) ;
    glGenBuffers(1, &vbo_boids) ; //creates buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boids) ;
    num_boid_vertices = 12 * state.boid_physics_objs.size();
    vertices = (Vtx*) malloc(num_boid_vertices * sizeof(Vtx)) ;
    glBufferData(GL_ARRAY_BUFFER, num_boid_vertices * sizeof(Vtx), vertices, GL_DYNAMIC_DRAW) ;

    glEnableVertexAttribArray(pos_attr) ;
    glEnableVertexAttribArray(col_attr) ;
    glEnableVertexAttribArray(nor_attr) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boids) ;
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), 0) ;
    glVertexAttribPointer(col_attr, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)(3 * sizeof(GLfloat))) ;
    glVertexAttribPointer(nor_attr, 3, GL_FLOAT, GL_TRUE, sizeof(Vtx), (void*)(6 * sizeof(GLfloat))) ;

    glBindVertexArray(vao_grid) ;
    glGenBuffers(1, &vbo_grid) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_grid) ;
    int grid_size;
    grid_xyz = gen_2d_grid(&grid_size, &num_grid_pts, 10, 1, Y_AXIS, 0.0f) ;
    //float* grid_xyz = gen_3d_grid(&grid_size, &num_grid_pts, 10, 1) ;
    glBufferData(GL_ARRAY_BUFFER, grid_size, grid_xyz, GL_STATIC_DRAW) ;
    glEnableVertexAttribArray(pos_attr) ;
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0) ;
    glVertexAttrib3f(col_attr, 1.0f, 0.0f, 0.0f) ;

    glBindVertexArray(vao_iboxes) ;
    glGenBuffers(1, &vbo_iboxes) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_iboxes) ;
    int num_iboxes = 1 ;
    int ibox_pt_s = 3 ;
    int ibox_s = 24 ; //12 lines, 2 vertices per line
    num_iboxes_pts = ibox_s * num_iboxes ;
    float ibox_vertices[num_iboxes_pts * ibox_pt_s] ;
    InvertedBox::render(state.iboxes, ibox_vertices) ;
    glBufferData(GL_ARRAY_BUFFER, sizeof(ibox_vertices), ibox_vertices, GL_STATIC_DRAW) ;
    glEnableVertexAttribArray(pos_attr) ;
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0) ;
    glVertexAttrib3f(col_attr, 1.0f, 1.0f, 1.0f) ;

    uni_model = glGetUniformLocation(shader_program, "model") ;
    uni_view = glGetUniformLocation(shader_program, "view") ;
    uni_proj = glGetUniformLocation(shader_program, "proj") ;
    glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(state.camera.view));
    glUniformMatrix4fv(uni_proj, 1, GL_FALSE, glm::value_ptr(state.camera.proj));
    uni_amb = glGetUniformLocation(shader_program, "ambient") ;
    glm::vec3 ambient_light (0.1f, 0.1f, 0.1f) ;
    glUniform3fv(uni_amb, 1, &ambient_light[0]) ;
    GLint uni_light = glGetUniformLocation(shader_program, "light_model") ;
    glm::vec3 light_pos (0.0f, 5.0f, 0.0f) ;
    glUniform3fv(uni_light, 1, &light_pos[0]) ;
    GLint uni_spec = glGetUniformLocation(shader_program, "spec_coefficient") ;
    GLfloat specular_coefficient = 10.0f ;
    glUniform1f(uni_spec, specular_coefficient) ;


    glEnable(GL_DEPTH_TEST) ;
}

void GraphicsEngine::render(GameState state, float t) {

    glm::vec3 white_light (1.0f,1.0f,1.0f) ;

    // actual render loop
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f) ; // Clear the screen to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
    glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(state.camera.view)) ;
    glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(state.camera.model));

    glBindVertexArray(vao_boids) ;
    render_old(state.boid_physics_objs, vertices); //TODO render with current velocity
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boids) ;
    glBufferData(GL_ARRAY_BUFFER, num_boid_vertices * sizeof(Vtx), vertices, GL_DYNAMIC_DRAW) ;
    glDrawArrays(GL_TRIANGLES, 0, num_boid_vertices) ;

    glBindVertexArray(vao_grid) ;
    glVertexAttrib3f(col_attr, 1.0f, 0.0f, 0.0f) ;
    glUniform3fv(uni_amb, 1, &white_light[0]) ;
    glDrawArrays(GL_LINES, 0, num_grid_pts) ;

    glBindVertexArray(vao_iboxes) ;
    glVertexAttrib3f(col_attr, 1.0f, 1.0f, 1.0f) ;
    glDrawArrays(GL_LINES, 0, num_iboxes_pts) ;

}

GraphicsEngine::~GraphicsEngine() {
    GraphicsEngine::initialized = false;
    glDeleteProgram(shader_program) ;
    glDeleteShader(fragment_shader) ;
    glDeleteShader(vertex_shader) ;
    glDeleteBuffers(1, &vbo_boids) ;
    glDeleteBuffers(1, &vbo_grid) ;
    glDeleteVertexArrays(1, &vao_boids) ;
    glDeleteVertexArrays(1, &vao_grid) ;
    free(grid_xyz);
    free(vertices);
}

// TODO instancing
// TODO take into account time between frames (delta)

void render_old(const vector<PhysicsObject> &boids, Vtx *vertices) {
    int s = (int) boids.size() ;
    Vtx vtx_0, vtx_1, vtx_2, vtx_3 ; //4 vertices per triangle
    PhysicsObject b ;
    V3 pos, acc ;
    V3 t_0, t_1, t_2, v_0, v_1, v_2, v_3 ;
    V3 e_0, e_1,n ;
    int v_i = 0 ;
    for (int b_i=0; b_i < s; ++b_i) {
        b = boids[b_i] ;
        pos = b.pos ;
        acc = b.acc ;
        if (is_zero_length(&acc)) { acc.x = 0.000001; } //acc,t_0 can't be zero or equal
        t_0 = b.vel ;
        if (is_zero_length(&t_0)) { t_0.y = 0.000001; }
        if (t_0 == acc) { acc.x += 0.000001; }
        cross(&t_0, &acc, &t_1) ;
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
        //debug_vtx(vtx_0) ;
        //debug_vtx(vtx_1) ;
        //debug_vtx(vtx_2) ;
        //debug_vtx(vtx_3) ;
    } ;
}
