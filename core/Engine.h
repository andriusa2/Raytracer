#ifndef ENGINE_H
#define ENGINE_H

#include "../utils/util_pack.h"
#include "./Scene.h"
#include "./Integrator.h"
class Engine {
public:
    Engine(Integrator * integrator, Config& config);
    void trace(unsigned int buffer[]);
    Scene * getScene();
private:
    Integrator * integrator;
    Scene * scene;
};

#endif