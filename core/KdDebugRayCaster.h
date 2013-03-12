#ifndef KD_DEBUG_RAY_CASTER_H
#define KD_DEBUG_RAY_CASTER_H
#include "./Integrator.h"
#include "../primitives/Triangle.h"

class KdDebugRayCaster : public Integrator {
public:
    KdDebugRayCaster(Config& config);
    void linkScene(Scene * myscene);
    void render(unsigned int * buffer);
    static Vector3D getCol(Triangle * tri, float u, float v);
    static Vector3D getNorm(Triangle * tri, float u, float v);
    static Vector3D getUV(Triangle * tri, float u, float v);
    void invalidateAvgs() {};
private:
    Vector3D trace(Ray & ray, int depth, float rho);
    int scrWidth;
    int scrHeight;
    int maxDepth;
    Camera * cam;
    Scene * scene;
};

#endif