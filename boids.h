#ifndef BOIDS_H
#define BOIDS_H
#include "vector3.h"
#include <vector>

struct Boid {
    unsigned int id ;
    V3 pos ;
    V3 vel ;
    V3 acc ;
} ;
void seperate(const double s, const Boid* b, const std::vector<Boid> bs, V3* sep) ;
void cohesion(const double x, const Boid* b, const std::vector<Boid> bs, V3* coh) ;
void align(const double x, const std::vector<Boid> bs, V3* ali) ;
V3 centroid(std::vector<Boid> bs) ;
#endif