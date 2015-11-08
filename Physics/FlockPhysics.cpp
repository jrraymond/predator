//
// Created by justin on 11/7/15.
//

#include "FlockPhysics.h"
#include "../GameConfig.h"

using std::vector;

/* returns force on physics object based on boid flocking rules */
V3 boid_forces(const PhysicsObject &b, vector<PhysicsObject> &bs, const FlockConfig config) {
    V3 f, sep, coh, ali = V3{0,0,0} ;
    seperate(config.seperate_factor, b, bs, &sep) ;
    cohesion(config.cohesion_factor, b, bs, &coh) ;
    align(config.align_factor, bs, &ali) ;
    add(&sep, &f, &f) ;
    add(&coh, &f, &f) ;
    add(&ali, &f, &f) ;
    return f;
}


/* calculates separation force
 * @s the distance to keep seperated from
 * @b the Boid to calculate the force on
 * @bs the boids in the flock
 * @sep a pointer to where the force vector will be written
 * */
void seperate(float s, const PhysicsObject &b, const vector<PhysicsObject> &bs, V3 *sep) {
    float d ; //distance between boid and other
    int c = 0 ; //number of boids near
    float ss = s * s ;
    V3 vd ; //distance vector
    V3 p = b.pos ; //position of boid
    for (int i = 0; i < bs.size(); ++i) {
        sub(&p, &bs[i].pos, &vd) ; //calculate distance vector
        d = mag2(&vd) ;
        if (d < ss and d != 0) {
            normalize(&vd) ;
            scale(1.0f / d, &vd) ; //scale by inverse of distance squared
            add(&vd, sep, sep) ;
            ++c ;
        }
    }
    if (c != 0) { scale(1.0f / c, sep) ; }
}
/* the cohesion force
 * @x the scaling factor
 * @b the Boid to calculate force for
 * @bs the boids in the flock
 * @coh a pointer to where the force vector will be written
 * */
void cohesion(float x, const PhysicsObject &b, const vector<PhysicsObject> &bs, V3 *coh) {
    V3 c = centroid(bs) ;
    unsigned long s = bs.size() ;
    sub(&c, &b.pos, coh) ;
    if (s != 0) { scale(1.0f / s * x, coh) ; }
}
/* the alignment force
 * @x the scaling factor
 * @bs the boids in the flock
 * @ali a ptr to where the force vector will be written
 * */
void align(float x, const vector<PhysicsObject> &bs, V3 *ali) {
    unsigned long s = bs.size() ;
    for (int i = 0; i < s; ++i) {
        add(&bs[i].vel, ali, ali) ;
    }
    if (s != 0) { scale(1.0f / s * x, ali) ; }
}
/* void avoid(float s, Boid* b, vector<Boid> bs) {  } */
/* Calculates the centroid of a vector of boids
 * */
V3 centroid(const vector<PhysicsObject>& bs) {
    float x = 0, y = 0, z = 0 ;
    V3 tmp ;
    float s = (float) bs.size() ;
    for (int i = 0; i < s; ++i) {
        tmp = bs[i].pos ;
        x += tmp.x ;
        y += tmp.y ;
        z += tmp.z ;
    }
    return V3 { x / s, y / s, z / s } ;
}


/** Computes the gravitational force on boid b from body m with mass mass and stores
* it in v.
*/
void gravity(float mass, V3* m, const PhysicsObject& b, V3* v) {
    V3 dist ;
    sub(m, &b.pos, &dist) ;
    normalize(&dist) ;
    float s = mag2(&dist) ;
    scale(1 / s * mass, &dist) ;
    add(&dist, v, v) ;
}
