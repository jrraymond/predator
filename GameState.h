//
// Created by justin on 11/7/15.
//

#ifndef FLOCK_HERD_SCHOOL_GAMEMANAGER_H
#define FLOCK_HERD_SCHOOL_GAMEMANAGER_H


#include "Player.h"
#include "Boid.h"
#include "GameConfig.h"
#include "Graphics/Camera.h"

using std::vector;

class GameState {
public:
    GameConfig config;
    Camera camera;
    Player player;
    vector<Boid> boids;
    vector<PhysicsObject> boid_physics_objs;
    vector<PhysicsObject> static_objs;
    float* grid_2d;
};


#endif //FLOCK_HERD_SCHOOL_GAMEMANAGER_H
