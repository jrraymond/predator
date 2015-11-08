//
// Created by justin on 11/7/15.
//

#include <iostream>
#include "Player.h"
#include "vector3.h"

std::ostream &operator<<(std::ostream &stream, const Player &p) {
    std::cout << "p{" << fromGLM(p.pos) << "} v{" << fromGLM(p.vel) << "} a{" << fromGLM(p.acc) << "}";
    return stream;
}
