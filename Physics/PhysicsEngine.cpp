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
    //state.boid_physics_objs[0].debug("boid: ");
    //std::cout << state.player << std::endl;
    for (int i=0; i<state.boid_physics_objs.size();++i) {
        PhysicsObject pobj (state.boid_physics_objs[i]);
        V3 boid_force = boid_forces(pobj, state.boid_physics_objs, state.config.flock);
        pobj.force = boid_force;
        V3 player_pos {state.player.pos.x, state.player.pos.y, state.player.pos.z};
        avoid(state.config.avoid_player_distance, state.config.avoid_player_strength, &player_pos, &pobj.pos, &pobj.force);
        bp_objs.push_back(pobj);
    }
    //std::cout << "bfc " << fromGLM(state.player.acc) << std::endl;
    //update the player force
    Player next_p (state.player);
    next_p.vel = state.player.vel + state.player.acc * (float) (t / 1000.0);
    float speed = glm::length(next_p.vel);
    if (speed > state.config.max_speed and speed != 0) { next_p.vel = glm::normalize(next_p.vel) * state.config.max_speed; }
    next_p.pos = state.player.pos + state.player.vel * (float) (t / 1000.0);
    next_p.acc = glm::vec3(0,0,0);
    std::cout << next_p << std::endl;
    //std::cout << "afc " << fromGLM(next_p.acc) << std::endl;

    /* Move all the boids */
    //TODO calculate collisions with player
    for (int i=0; i<state.boid_physics_objs.size();++i) {
        bp_objs[i].move(t, state.config.flock);
        V3 bounce {0,0,0} ;
        for (auto ibox : state.iboxes) {
            bounce = ibox.collides(state.boid_physics_objs[i].pos, bp_objs[i].pos);
            if (! is_zero_length(&bounce)) { //if there is a collision, reset and turn around
                bp_objs[i].pos = state.boid_physics_objs[i].pos;
                bp_objs[i].vel = bounce;
                bp_objs[i].acc = V3 {0,0,0};
            }
        }
    }

    //bp_objs[0].debug("boid 0: ");
    GameState next_state (state);
    next_state.boid_physics_objs = bp_objs;
    next_state.player = next_p;

    return next_state;
}

void avoid(const float d, const float s, const V3 *avoidee, const V3 *avoider, V3 *force) {
    V3 dv;
    sub(avoider, avoidee, &dv);
    float m2 = mag2(&dv);
    if (m2 <= d*d) {
        float m = std::sqrt(m2);
        scale((d - m)/m,&dv);
        add(&dv, force, force);
        //std::cout << "avoid force: " << dv << std::endl;
    }
}
