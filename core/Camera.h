#ifndef CAMERA_H
#define CAMERA_H

#include "../utils/util_pack.h"

class Camera {
public:
    enum {
        LEFT = 1,
        RIGHT = 1 << 1,
        FRONT = 1 << 2,
        BACK = 1 << 3,
        UP = 1 << 4,
        DOWN = 1 << 5
    };
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
    void getDirection(Ray & ray, float x, float y, float whr);
    Vector3D getPos();
    void go(int mask);
    /* turn by xz radians in the front x right panel
    * turn by yz radians in the front x top panel
    */
    void turn(float xz, float yz);
    void changeSpeed(int augment);
    bool invalidate();
    friend std::ostream& operator<< (std::ostream &out, const Camera & cam);
private:
    float speed;
    bool inv;
    Vector3D up;
    Vector3D right;
    Vector3D front;
    Vector3D position;
};

#endif