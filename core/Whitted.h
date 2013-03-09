#ifndef WHITTED_H
#define WHITTED_H

#include "../utils/util_pack.h"
#include "../utils/Config.h"
#include "./Camera.h"
#include "./Scene.h"
#include "./Integrator.h"
class Whitted : public Integrator {
public:
    Whitted(Config& config);
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