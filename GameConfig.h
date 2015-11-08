//
// Created by justin on 11/7/15.
//

#ifndef FLOCK_HERD_SCHOOL_GAMECONFIG_H
#define FLOCK_HERD_SCHOOL_GAMECONFIG_H



class FlockConfig {
public:
    float seperate_factor;
    float align_factor;
    float cohesion_factor;
    float max_pos;
    float max_vel;
    float max_acc;
};
class GameConfig {
public:
    FlockConfig flock;
};
#endif //FLOCK_HERD_SCHOOL_GAMECONFIG_H
