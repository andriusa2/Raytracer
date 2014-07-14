#include "./Engine.h"
#include <Windows.h>
Engine::Engine(Integrator * integr, Config & config) {
    if (config.getIntByName("threads") > 0)
        omp_set_num_threads(config.getIntByName("threads"));
    else omp_set_num_threads(1);
    scene = new Scene(config);
    integrator = integr;
    integrator->linkScene(scene);
    __dsfmt_sz = new dsfmt_t[8];
    for (int i = 0; i < 8; i++)
        dsfmt_init_gen_rand(__dsfmt_sz+i, 0);
}

void Engine::trace(unsigned int buffer[]) {
    float start;
#pragma omp master
    {
        start = omp_get_wtime();
    }
    integrator->render(buffer);
#pragma omp master
    {
        LogDefault->criticalOutValue("Rendering took", (omp_get_wtime() - start)*1000.0);
    }
}

Scene * Engine::getScene() {
    return scene;
}

dsfmt_t* __dsfmt_sz;