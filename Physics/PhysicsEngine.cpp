//
// Created by justin on 11/7/15.
//

#include "PhysicsEngine.h"
#include "FlockPhysics.h"

using std::vector;

GameState update_physics(GameState state, float t) {

    /* Calculate forces on all boids */
    vector<PhysicsObject> bp_objs;
    bp_objs.reserve(state.boid_physics_objs.size()) ;

    for (int i=0; i<state.boid_physics_objs.size();++i) {
        PhysicsObject pobj (state.boid_physics_objs[i]);
        V3 boid_force = boid_forces(pobj, state.boid_physics_objs, state.config.flock);
        pobj.force = boid_force;
        //TODO player force, gravity force etc etc
        bp_objs.push_back(pobj);
    }

    /* Move all the boids */
    //TODO calculate collisions
    for (int i=0; i<state.boid_physics_objs.size();++i) {
        bp_objs[i].move(t);
    }

    GameState next_state (state);
    next_state.boid_physics_objs = bp_objs;

    return next_state;
}
