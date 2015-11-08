//
// Created by justin on 11/7/15.
//

#include "PhysicsObject.h"
#include "../GameConfig.h"

void PhysicsObject::move(float t, FlockConfig config) {
    acc = force;
    scale(1.0 / mass,&acc);  // f = ma
    scale(t,&acc);           // a*t
    cap(config.max_acc,&acc);
    add(&acc,&vel,&vel);     // v = v + a*t
    scale(t,&vel);           // v*t
    cap(config.max_vel,&vel);
    add(&vel,&pos,&pos);     // p = p + v*t
    cap(config.max_pos,&pos);
}

void PhysicsObject::debug(string msg) {
    std::cout << msg << " p" << pos << " v" << vel << " a" << acc << " m" << mass << std::endl;
};
