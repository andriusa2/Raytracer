#ifndef RAY_H
#define RAY_H

#include "./Vector3D.h"
#include "./constants.h"
struct Ray {
    Ray():origin(V3D_BLANK),direction(V3D_BLANK) {};
    Ray(const Vector3D& o, const Vector3D& d):
        origin(o), direction(d) {};
    Vector3D& getO(){return origin;}
    Vector3D& getD(){return direction;}
    Vector3D origin;
    Vector3D direction;
};

#endif