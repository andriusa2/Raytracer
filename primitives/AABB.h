#ifndef AABB_H
#define AABB_H

#include "../utils/Vector3D.h"

struct AABB {
    Vector3D left, right;
    bool Contains(Vector3D & a) {for (int i = 0; i < 3; i++) if (a[i] < left[i] || a[i] > right[i]) return false; return true;}
    AABB():left(), right() {};
    AABB(Vector3D left_, Vector3D right_): left(left_), right(right_) {};
};

#endif