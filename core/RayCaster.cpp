#include "./RayCaster.h"
#include <omp.h>
RayCaster::RayCaster(Config& config):
    scrWidth(config.getIntByName("width")),
    scrHeight(config.getIntByName("height")),
    maxDepth(config.getIntByName("max_depth"))
{
    LogDefault->line();
    LogDefault->outString("Initialized simple RayCaster with\n");
    LogDefault->outValue("width",scrWidth);
    LogDefault->outValue("height",scrHeight);
    LogDefault->outValue("max_depth",maxDepth);
    if (config.getIntByName("accumulator") != 0)
        avgBuffer = new Vector3D [scrWidth * scrHeight];
    else avgBuffer = 0;
    sampleNo = 0;
    LogDefault->outValue("accumulator",avgBuffer!=0);
}

void RayCaster::linkScene(Scene * myscene) {
    scene = myscene;
    cam = scene->getCamera();
}

void RayCaster::render(unsigned int * buffer) {
    if (avgBuffer != 0) 
        return renderAcc(buffer);
    Ray ray;
    omp_set_num_threads(7);
#pragma omp parallel for schedule(dynamic) private(ray)
    for (int y = 0; y < scrHeight; y++)
        for (int x = 0; x < scrWidth; x++) {
            ray.origin = cam->getPos();
            cam->getDirection(ray, (float(x)/(float(scrWidth))), (float(y)/float(scrHeight)), float(scrHeight)/float(scrWidth));
            float dist = 0;
            buffer[y * scrWidth + x] =
                trace(ray, 0, dist, 1).toRGBCol();
        }
}

void RayCaster::renderAcc(unsigned int * buffer) {
    Ray ray;
    sampleNo ++;
    omp_set_num_threads(7);
#pragma omp parallel for schedule(dynamic) private(ray)
    for (int y = 0; y < scrHeight; y++)
        for (int x = 0; x < scrWidth; x++) {
            ray.origin = cam->getPos();
            cam->getDirection(ray,
                ((float(x)+0.5f-GET_RND(omp_get_thread_num()))/(float(scrWidth))),
                ((float(y)+0.5f-GET_RND(omp_get_thread_num()))/float(scrHeight)),
                float(scrHeight)/float(scrWidth));
            float dist = 0;
            Vector3D tmpCol = trace(ray, 0, dist, 1);
            
            avgBuffer[y*scrWidth+x] += tmpCol;
            buffer[y * scrWidth + x] =
                (avgBuffer[y * scrWidth + x] * (1.0f/float(sampleNo))).toRGBCol();
        }
}
Vector3D RayCaster::trace(Ray & ray, int depth, float & dist, float rho) {
    if (depth == maxDepth) return V3D_BLANK;

    Triangle * prim;
    dist = 0;
    float u;
    float v;
    prim = scene->intersect(ray, dist, u, v);
    if (!prim) return V3D_BLANK;
    
    if (prim->getMat()->isEmitter()) {
        return prim->getMat()->getColor();
    }
    ray.origin += ray.direction * dist;
    ray.direction = -ray.direction;

    return prim->getMat()->getColor() *
        scene->accumulateLight(ray, prim->getNormal(u, v));
}
