//
// Created by justin on 11/7/15.
//

#ifndef FLOCK_HERD_SCHOOL_CAMERA_H
#define FLOCK_HERD_SCHOOL_CAMERA_H


#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>

class Camera {
public:
    glm::mat4 model = glm::mat4();
    glm::mat4 view = glm::mat4();
    glm::mat4 proj = glm::mat4();
};


#endif //FLOCK_HERD_SCHOOL_CAMERA_H
