#include "./Camera.h"

Camera::Camera() {
    set(V3D_BLANK, Vector3D(0,0,1), Vector3D(0,1,0));
    LogDefault.outValue("IntializedCamera",*this);
}

Camera::Camera(const Vector3D & pos,
        const Vector3D & lookAt,
        const Vector3D & top) {
    set(pos, lookAt, top);
}

Camera::Camera(const Vector3D & pos,
        const Vector3D & u,
        const Vector3D & r,
        const Vector3D & f):
up(u), position(pos), right(r), front(f)
{

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
}

std::ostream& operator<< (std::ostream &out, const Camera & cam) {
    out << "Camera: \n\tPos=" << cam.position <<
        "\n\tUp=" << cam.up <<
        "\n\tFront=" << cam.front <<
        "\n\tRight=" << cam.right;
}

void Camera::getDirection(Ray & ray, float x, float y) {
    ray.direction = front + (x - 0.5f) * right + (y - 0.5f) * up;
    LogDefault.outValue("|ray.direction|",ray.direction.length());
    ray.direction.normalize();
}
Vector3D Camera::getPos() {
    return getPos();
}
