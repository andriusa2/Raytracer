#include "./DebugRayCaster.h"
DebugRayCaster::DebugRayCaster(Config& config, DebugFunction dbg):
    scrWidth(config.getIntByName("width")),
    scrHeight(config.getIntByName("height")),
    maxDepth(config.getIntByName("max_depth")),
    __fun(dbg)
{
    LogDefault->line();
    LogDefault->outString("Initialized debug raycaster with\n");
    LogDefault->outValue("width",scrWidth);
    LogDefault->outValue("height",scrHeight);
    LogDefault->outValue("max_depth",maxDepth);

}

void DebugRayCaster::linkScene(Scene * myscene) {
    scene = myscene;
    cam = scene->getCamera();
}

void DebugRayCaster::render(unsigned int * buffer) {
    Ray ray;
    for (int y = 0; y < scrHeight; y++)
        for (int x = 0; x < scrWidth; x++) {
            ray.origin = cam->getPos();
            cam->getDirection(ray, (float(x)/(float(scrWidth))), (float(y)/float(scrHeight)));
            buffer[y * scrWidth + x] =
                trace(ray, 0, 1).toRGBCol();
        }
}

Vector3D DebugRayCaster::trace(Ray & ray, int depth, float rho) {
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

    return (*__fun)(prim,0,0);
}

Vector3D DebugRayCaster::getCol(Triangle * tri, float u, float v) {
    return tri->getMat()->getColor(u,v);
}

Vector3D DebugRayCaster::getNorm(Triangle * tri, float u, float v) {
    return 0.5f * (V3D_WHITE + tri->getNormal(u,v));
}
Vector3D DebugRayCaster::getUV(Triangle * tri, float u, float v) {
    return Vector3D(u, v, 0);
}