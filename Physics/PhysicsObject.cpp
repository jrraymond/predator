//
// Created by justin on 11/7/15.
//

#include "PhysicsObject.h"

void PhysicsObject::move(float t) {
    acc = force;
    scale(1.0 / mass,&acc);  // f = ma
    scale(t,&acc);           // a*t
    add(&acc,&vel,&vel);     // v = v + a*t
    scale(t,&vel);           // v*t
    add(&vel,&pos,&pos);     // p = p + v*t
}
