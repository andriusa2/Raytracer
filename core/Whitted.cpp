#include "./Whitted.h"
#include <Windows.h>
Whitted::Whitted(Config& config):
    scrWidth(config.getIntByName("width")),
    scrHeight(config.getIntByName("height")),
    maxDepth(config.getIntByName("max_depth"))
{
    LogDefault->line();
    LogDefault->outString("Initialized whitted renderer with\n");
    LogDefault->outValue("width",scrWidth);
    LogDefault->outValue("height",scrHeight);
    LogDefault->outValue("max_depth",maxDepth);

}

void Whitted::linkScene(Scene * myscene) {
    scene = myscene;
    cam = scene->getCamera();
}

void Whitted::render(unsigned int * buffer) {
    int start = GetTickCount();
    Ray ray;
    for (int y = 0; y < scrHeight; y++)
        for (int x = 0; x < scrWidth; x++) {
            ray.origin = cam->getPos();
            cam->getDirection(ray, (float(x)/(float(scrWidth))), (float(y)/float(scrHeight)));
            buffer[y * scrWidth + x] =
                trace(ray, 0, 1).toRGBCol();
        }


    LogDefault->criticalOutValue("Rendering took",GetTickCount()-start);

}

Vector3D Whitted::trace(Ray & ray, int depth, float rho) {
    if (depth == maxDepth) return V3D_BLANK;

    Triangle * prim;
    float dist = 0;
    prim = scene->intersect(ray, dist);
    if (!prim) return V3D_BLANK;
    
    if (prim->getMat()->isEmitter()) {
        return prim->getMat()->getColor();
    }
    ray.origin += ray.direction * dist;
    ray.direction = -ray.direction;

    return prim->getMat()->getColor() *
        scene->accumulateLight(ray, prim->getNormal());
}
