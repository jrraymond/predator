//
// Created by justin on 11/7/15.
//

#ifndef FLOCK_HERD_SCHOOL_FLOCKPHYSICS_H
#define FLOCK_HERD_SCHOOL_FLOCKPHYSICS_H

#include "../Boid.h"
#include "../GameConfig.h"


using std::vector;

V3 boid_forces(const PhysicsObject &b, vector<PhysicsObject> &bs, const FlockConfig config);

void seperate(float s, const PhysicsObject &b, const vector<PhysicsObject> &bs, V3 *sep) ;
void cohesion(float x, const PhysicsObject &b, const vector<PhysicsObject> &bs, V3 *coh) ;
void align(float x, const vector<PhysicsObject> &bs, V3 *ali) ;
V3 centroid(const vector<PhysicsObject> &bs) ;

#endif //FLOCK_HERD_SCHOOL_FLOCKPHYSICS_H
