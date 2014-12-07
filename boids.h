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
void update_flock(std::vector<Boid> bs) ;
void update_boid(Boid* b, std::vector<Boid> bs) ;
void seperate(float s, const Boid* b, const std::vector<Boid> bs, V3* sep) ;
void cohesion(float x, const Boid* b, const std::vector<Boid> bs, V3* coh) ;
void align(float x, const std::vector<Boid> bs, V3* ali) ;
V3 centroid(std::vector<Boid> bs) ;
void debug_boid(Boid* b) ;
void orbit(std::vector<Boid> &bs, V3 v, float angle) ;

#endif