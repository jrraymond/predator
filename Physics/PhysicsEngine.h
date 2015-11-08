//
// Created by justin on 11/7/15.
//

#ifndef FLOCK_HERD_SCHOOL_PHYSICSENGINE_H
#define FLOCK_HERD_SCHOOL_PHYSICSENGINE_H


#include "../GameState.h"

class PhysicsEngine {

};

GameState update_physics(GameState state, float t) ;

void avoid(const float d, const float s, const V3 *avoidee, const V3 *avoider, V3 *force);



#endif //FLOCK_HERD_SCHOOL_PHYSICSENGINE_H
