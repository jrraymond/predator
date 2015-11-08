//
// Created by justin on 11/7/15.
//

#ifndef FLOCK_HERD_SCHOOL_PHYSICSOBJECT_H
#define FLOCK_HERD_SCHOOL_PHYSICSOBJECT_H

#include "../vector3.h"

class PhysicsObject {
public:
    float mass ;    // kg
    V3 pos ;        // m
    V3 vel ;        // m/s
    V3 acc ;        // m/s^2
    V3 force ;      // force acting on object
    float radius ;  // everything is a sphere for now
    void move(float t);
};


#endif //FLOCK_HERD_SCHOOL_PHYSICSOBJECT_H
