#include "./Engine.h"

Engine::Engine(Integrator * integr, Config & config) {
    scene = new Scene(config);
    integrator = integr;
    integrator->linkScene(scene);
    __dsfmt_sz = new dsfmt_t[3];
    dsfmt_init_gen_rand(__dsfmt_sz, 0);
}

void Engine::trace(unsigned int buffer[]) {
    integrator->render(buffer);
}

dsfmt_t* __dsfmt_sz;