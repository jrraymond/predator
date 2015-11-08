//
// Created by justin on 11/7/15.
//

#ifndef FLOCK_HERD_SCHOOL_PLAYER_H
#define FLOCK_HERD_SCHOOL_PLAYER_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <iosfwd>

class Player {
public:
    glm::vec3 pos ;
    glm::vec3 vel ;
    glm::vec3 acc ;
    float h_angle ;
    float v_angle ;
    float fov ;
};
std::ostream &operator<<(std::ostream &stream, const Player &p);

#endif //FLOCK_HERD_SCHOOL_PLAYER_H
