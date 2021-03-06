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
    void renderPack(unsigned int * buffer);
    void renderAccPack(unsigned int * buffer);
    void invalidateAvgs();
private:
    Vector3D trace(Ray & ray, int depth, float & dist, float rho);
    void tracePack(Ray ray[], int depth, float & dist, float rho, Vector3D cols[]);
    void renderAcc(unsigned int * buffer);
    int scrWidth;
    int scrHeight;
    int maxDepth;
    Vector3D * avgBuffer;
    int sampleNo;
    int ss;
    bool packet;
    Camera * cam;
    Scene * scene;
};

#endif