#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "../utils/util_pack.h"
#include "../utils/Config.h"
#include "./Camera.h"
#include "./Scene.h"
#include "./Integrator.h"
class RayCaster : public Integrator {
public:
    RayCaster(Config& config);
    void linkScene(Scene * myscene);
    void render(unsigned int * buffer);

private:
    Vector3D trace(Ray & ray, int depth, float rho);
    int scrWidth;
    int scrHeight;
    int maxDepth;
    Camera * cam;
    Scene * scene;
};

#endif