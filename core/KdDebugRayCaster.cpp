#include "./KdDebugRayCaster.h"
KdDebugRayCaster::KdDebugRayCaster(Config& config):
    scrWidth(config.getIntByName("width")),
    scrHeight(config.getIntByName("height")),
    maxDepth(config.getIntByName("max_depth"))
{
    LogDefault->line();
    LogDefault->outString("Initialized kdtree debug raycaster with\n");
    LogDefault->outValue("width",scrWidth);
    LogDefault->outValue("height",scrHeight);
    LogDefault->outValue("max_depth",maxDepth);

}

void KdDebugRayCaster::linkScene(Scene * myscene) {
    scene = myscene;
    cam = scene->getCamera();
}

void KdDebugRayCaster::render(unsigned int * buffer) {
    Ray ray;
    for (int y = 0; y < scrHeight; y++)
        for (int x = 0; x < scrWidth; x++) {
            ray.origin = cam->getPos();
            cam->getDirection(ray, (float(x)/(float(scrWidth))), (float(y)/float(scrHeight)), float(scrHeight)/float(scrWidth));
            buffer[y * scrWidth + x] =
                trace(ray, 0, 1).toRGBCol();
        }
}

Vector3D KdDebugRayCaster::trace(Ray & ray, int depth, float rho) {
    if (depth == maxDepth) return V3D_BLANK;

    float dist = INF;
    int tval = scene->getTree().debugIntersect(ray, dist);
    if (!tval) return V3D_BLANK;
    return (float(tval)/30) * V3D_WHITE;
}

Vector3D KdDebugRayCaster::getCol(Triangle * tri, float u, float v) {
    return tri->getMat()->getColor(u,v);
}

Vector3D KdDebugRayCaster::getNorm(Triangle * tri, float u, float v) {
    return 0.5f * (V3D_WHITE + tri->getNormal(u,v));
}
Vector3D KdDebugRayCaster::getUV(Triangle * tri, float u, float v) {
    return Vector3D(u, v, 0);
}