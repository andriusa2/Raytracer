#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "./Scene.h"
#include "./Camera.h"

class Integrator {
public:
    virtual void linkScene(Scene *) = 0;
    virtual void render(unsigned int * buffer) = 0;
};

#endif