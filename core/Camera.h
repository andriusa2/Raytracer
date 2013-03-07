#ifndef CAMERA_H
#define CAMERA_H

#include "../utils/util_pack.h"

class Camera {
public:
    Camera();
    Camera(const Vector3D & pos,
        const Vector3D & lookAt,
        const Vector3D & top);
    Camera(const Vector3D & pos,
        const Vector3D & u,
        const Vector3D & r,
        const Vector3D & f);
    void set(const Vector3D & pos,
        const Vector3D & lookAt,
        const Vector3D & top);
    void set(const Vector3D & pos,
        const Vector3D & u,
        const Vector3D & r,
        const Vector3D & f);
    void getRay(Ray & ray, float x, float y);
    Vector3D& getPos();
    friend std::ostream& operator<< (std::ostream &out, const Camera & cam);
private:
    Vector3D up;
    Vector3D right;
    Vector3D front;
    Vector3D position;
};

#endif