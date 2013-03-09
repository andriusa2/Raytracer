#include "./Engine.h"
#include <Windows.h>
Engine::Engine(Integrator * integr, Config & config) {
    scene = new Scene(config);
    integrator = integr;
    integrator->linkScene(scene);
    __dsfmt_sz = new dsfmt_t[3];
    dsfmt_init_gen_rand(__dsfmt_sz, 0);
}

void Engine::trace(unsigned int buffer[]) {
    
    int start = GetTickCount();
    integrator->render(buffer);
    
    LogDefault->criticalOutValue("Rendering took",GetTickCount()-start);
}

dsfmt_t* __dsfmt_sz;