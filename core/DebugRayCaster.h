#ifndef DEBUG_RAY_CASTER_H
#define DEBUG_RAY_CASTER_H
#include "./Integrator.h"
#include "../primitives/Triangle.h"
typedef Vector3D(*DebugFunction) (Triangle*, float, float);

class DebugRayCaster : public Integrator {
public:
    DebugRayCaster(Config& config, DebugFunction dbg = DebugRayCaster::getCol);
    void linkScene(Scene * myscene);
    void render(unsigned int * buffer);
    static Vector3D getCol(Triangle * tri, float u, float v);
    static Vector3D getNorm(Triangle * tri, float u, float v);
    static Vector3D getUV(Triangle * tri, float u, float v);

private:
    Vector3D trace(Ray & ray, int depth, float rho);
    int scrWidth;
    int scrHeight;
    int maxDepth;
    Camera * cam;
    Scene * scene;
    DebugFunction __fun;
};

#endif