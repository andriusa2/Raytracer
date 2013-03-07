#ifndef RAY_H
#define RAY_H

#include "./Vector3D.h"

struct Ray {
    Ray(const Vector3D& o, const Vector3D& d):
        origin(o), direction(d) {};

    Vector3D origin;
    Vector3D direction;
};

#endif