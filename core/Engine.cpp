#include "./Engine.h"
#include <Windows.h>
Engine::Engine(Integrator * integr, Config & config) {
    scene = new Scene(config);
    integrator = integr;
    integrator->linkScene(scene);
    __dsfmt_sz = new dsfmt_t[8];
    for (int i = 0; i < 8; i++)
        dsfmt_init_gen_rand(__dsfmt_sz+i, 0);
}

void Engine::trace(unsigned int buffer[]) {
    
    int start = GetTickCount();
    integrator->render(buffer);
    
    LogDefault->criticalOutValue("Rendering took",GetTickCount()-start);
}

dsfmt_t* __dsfmt_sz;