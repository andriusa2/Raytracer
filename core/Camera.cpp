#include "./Camera.h"

Camera::Camera() {
    speed = DEFCAMSPEED;
    inv = false;
    set(V3D_BLANK, Vector3D(0,0,1), Vector3D(0,1,0));
    LogDefault->outValue("IntializedCamera",*this);
}

Camera::Camera(const Vector3D & pos,
        const Vector3D & lookAt,
        const Vector3D & top) {
    speed = DEFCAMSPEED;
    inv = false;
    set(pos, lookAt, top);
}

Camera::Camera(const Vector3D & pos,
        const Vector3D & u,
        const Vector3D & r,
        const Vector3D & f):
up(u), position(pos), right(r), front(f), speed(DEFCAMSPEED)
{
    inv = false;
}


void Camera::set(const Vector3D & pos,
        const Vector3D & lookAt,
        const Vector3D & top) {
    position = pos;
    front = lookAt - pos;
    front.normalize();
    up = top;
    up.normalize();
    right = up.cross(front);
    right.normalize();
    up = front.cross(right);
    up.normalize();
    LogDefault->outValue("Camera reset", *this);
}

void Camera::set(const Vector3D & pos, const Vector3D & u, const Vector3D & r,
                 const Vector3D & f) {
    position = pos;
    up = u;
    right = r;
    front = f;
    right = up.cross(front);
    up = front.cross(right);
    right.normalize();
    front.normalize();
    up.normalize();
    LogDefault->outValue("Camera precise reset", *this);
}
std::ostream& operator<< (std::ostream &out, const Camera & cam) {
    out << "Camera: \n\tPos=" << cam.position <<
        "\n\tUp=" << cam.up <<
        "\n\tFront=" << cam.front <<
        "\n\tRight=" << cam.right;
    return out;
}
// hwr => height/width
void Camera::getDirection(Ray & ray, float x, float y, float whr) {
    ray.direction = front + (x - 0.5f) * (2.2f/whr) * right + (0.5f - y) * 2.2f * up;
    //LogDefault->outValue("|ray.direction|",ray.direction.length());
    ray.direction.normalize();
}
Vector3D Camera::getPos() {
    return position;
}

void Camera::changeSpeed(int augment) {
    if (augment > 0) 
        speed *= 1.1;
    else speed *= 0.9;
}
bool Camera::invalidate() {
    if (inv) {
        inv = false;
        return true;
    }
    return false;
}

void Camera::go(int mask) {
    if (mask & LEFT)
        position -= right * speed;
    if (mask & RIGHT)
        position += right * speed;
    if (mask & FRONT)
        position += front * speed;
    if (mask & BACK)
        position -= front * speed;
    if (mask & UP)
        position += up * speed;
    if (mask & DOWN)
        position -= up * speed;

    inv = mask != 0;
}

void Camera::turn(float xz, float yz) {
    

}