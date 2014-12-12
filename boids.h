#ifndef BOIDS_H
#define BOIDS_H
#include "vector3.h"
#include <vector>
#include <iostream>

struct Boid {
    V3 pos ;
    V3 vel ;
    V3 acc ;
} ;
using std::vector ;
void update_flock(vector<Boid> &bs, int x_max, int y_max, int z_max) ;
void update_boid(Boid* b, vector<Boid> &bs, int x_max, int y_max, int z_max) ;
void seperate(float s, const Boid* b, const vector<Boid> &bs, V3* sep) ;
void cohesion(float x, const Boid* b, const vector<Boid> &bs, V3* coh) ;
void align(float x, const vector<Boid> &bs, V3* ali) ;
V3 centroid(const vector<Boid> &bs) ;
void debug_boid(Boid* b) ;
void orbit(vector<Boid> &bs, V3 v, float angle) ;
void gravity(float mass, V3* m, const Boid* b, V3* v) ;

#endif