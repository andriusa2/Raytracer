#ifndef AABB_H
#define AABB_H

#include "../utils/Vector3D.h"

struct AABB {
    Vector3D left, right;

    AABB():left(), right() {};
    AABB(Vector3D left_, Vector3D right_): left(left_), right(right_) {};
};

#endif