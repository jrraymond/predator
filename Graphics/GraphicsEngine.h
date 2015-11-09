//
// Created by justin on 11/7/15.
//

#ifndef FLOCK_HERD_SCHOOL_GRAPHICSENGINE_H
#define FLOCK_HERD_SCHOOL_GRAPHICSENGINE_H


#include <GL/glew.h>
#include "../GameState.h"
#include "../object.h"

using std::string;

class GraphicsEngine {
public:

    GLuint vertex_shader, fragment_shader, shader_program;
    GLuint pos_attr, col_attr, nor_attr;
    GLuint vao_boids, vao_grid, vao_iboxes;
    GLuint vbo_boids, vbo_grid, vbo_iboxes;
    GLint uni_view, uni_model, uni_proj, uni_amb;
    float* grid_xyz;
    int num_boid_vertices, num_grid_pts, num_iboxes_pts;
    Vtx* vertices;


    GraphicsEngine(GameState state);

    void render(GameState state, float t) ;

    ~GraphicsEngine();

    static bool initialized;
};
void render_player(const Player p, Vtx* vertices);
void render_old(const vector<PhysicsObject> &boids, Vtx *vertices);


#endif //FLOCK_HERD_SCHOOL_GRAPHICSENGINE_H
