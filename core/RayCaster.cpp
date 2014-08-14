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
    LogDefault->outValue("ompnumthreads",omp_get_max_threads());
    if (config.getIntByName("accumulator") != 0)
        avgBuffer = new Vector3D [scrWidth * scrHeight];
    else avgBuffer = 0;
    packet = config.getIntByName("packet") != 0;
    sampleNo = 0;
    LogDefault->outValue("accumulator",avgBuffer!=0);
}

void RayCaster::linkScene(Scene * myscene) {
    scene = myscene;
    cam = scene->getCamera();
}

void RayCaster::renderPack(unsigned int * buffer) {
    if (avgBuffer != 0) {
        if (cam->invalidate()){
            invalidateAvgs();
        }
        return renderAcc(buffer);
    }
    Ray rays[4];
    int sq_h = scrHeight / 2;
    int sq_w = scrWidth / 2;
#pragma omp parallel for schedule(dynamic) private(rays)
//#pragma omp parallel for private(rays)
    for (int y = 0; y < sq_h; y++)
        for (int x = 0; x < sq_w; x++)
        {
            for (int dy = 0; dy < 2; dy++)
            for (int dx = 0; dx < 2; dx++)
            {
                rays[dx * 2 + dy].origin = cam->getPos();
                cam->getDirection(rays[dx * 2 + dy], (float(x * 2 + dx) / (float(scrWidth))), (float(y * 2 + dy) / float(scrHeight)), float(scrHeight) / float(scrWidth));
            }
            float dist = 0;
            Vector3D cols[4];
            //if (x == 151 && y == 0)
            //    LogDefault->outString("Derp");
            tracePack(rays, 0, dist, 1, cols);
            for (int dy = 0; dy < 2; dy++)
                for (int dx = 0; dx < 2; dx++)
            buffer[(y * 2 + dy) * scrWidth + x * 2 + dx] =
                cols[dx * 2 + dy].toRGBCol();
        }
}

void RayCaster::renderAccPack(unsigned int * buffer) {
    Ray rays[4];
    sampleNo++;
    int sq_h = scrHeight / 2;
    int sq_w = scrWidth / 2;
#pragma omp parallel for schedule(dynamic) private(rays)
    for (int y = 0; y < sq_h; y++)
        for (int x = 0; x < sq_w; x++)
        {

            for (int dy = 0; dy < 2; dy++)
            for (int dx = 0; dx < 2; dx++)
            {
                
                rays[dx*2+dy].origin = cam->getPos();
                cam->getDirection(rays[dx*2+dy],
                    ((float(x * 2 + dx) + 0.5f - GET_RND(omp_get_thread_num())) / (float(scrWidth))),
                    ((float(y * 2 + dy) + 0.5f - GET_RND(omp_get_thread_num())) / float(scrHeight)),
                    float(scrHeight) / float(scrWidth));
            }
            float dist = 0;
            Vector3D cols[4];
            tracePack(rays, 0, dist, 1, cols);
            for (int dy = 0; dy < 2; dy++)
                for (int dx = 0; dx < 2; dx++)
                {
                int id = (y * 2 + dy) * scrWidth + x * 2 + dx;
                if (sampleNo == 1)
                    avgBuffer[id] = cols[dx*2+dy];
                else avgBuffer[id] += cols[dx * 2 + dy];
                buffer[id] =
                    (avgBuffer[id] * (1.0f / float(sampleNo))).toRGBCol();
            }
        }
}

void RayCaster::render(unsigned int * buffer) {
    if (avgBuffer != 0) {
        if (cam->invalidate()){
            invalidateAvgs();
        }
        if (packet)
            return renderAccPack(buffer);
        return renderAcc(buffer);
    }
    if (packet)
        return renderPack(buffer);
    Ray ray;
    int sq_h = scrHeight / 2;
    int sq_w = scrWidth / 2;
#pragma omp parallel for schedule(dynamic) private(ray)
    for (int y = 0; y < sq_h; y++)
        for (int x = 0; x < sq_w; x++)
            for (int dy = 0; dy < 2; dy++)
                for (int dx = 0; dx < 2; dx++)
                {
        ray.origin = cam->getPos();
        cam->getDirection(ray, (float(x * 2 + dx) / (float(scrWidth))), (float(y * 2 + dy) / float(scrHeight)), float(scrHeight) / float(scrWidth));
        float dist = 0;
        buffer[(y * 2 + dy) * scrWidth + x * 2 + dx] =
            trace(ray, 0, dist, 1).toRGBCol();
                }
}

void RayCaster::renderAcc(unsigned int * buffer) {
    Ray ray;
    sampleNo ++;
    int sq_h = scrHeight / 2;
    int sq_w = scrWidth / 2;
#pragma omp parallel for schedule(dynamic) private(ray)
    for (int y = 0; y < sq_h; y++)
        for (int x = 0; x < sq_w; x++)
            for (int dy = 0; dy < 2; dy++)
                for (int dx = 0; dx < 2; dx++)
                {
            int id = (y * 2 + dy) * scrWidth + x * 2 + dx;
            ray.origin = cam->getPos();
            cam->getDirection(ray,
                ((float(x * 2 + dx)+0.5f-GET_RND(omp_get_thread_num()))/(float(scrWidth))),
                ((float(y * 2 + dy)+0.5f-GET_RND(omp_get_thread_num()))/float(scrHeight)),
                float(scrHeight)/float(scrWidth));
            float dist = 0;
            Vector3D tmpCol = trace(ray, 0, dist, 1);
            if (sampleNo == 1)
                avgBuffer[id] = tmpCol;
            else avgBuffer[id] += tmpCol;
            buffer[id] =
                (avgBuffer[id] * (1.0f/float(sampleNo))).toRGBCol();
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

void RayCaster::tracePack(Ray rays[], int depth, float & dist, float rho, Vector3D cols[]) {
    if (depth == maxDepth) return;

    Triangle * prim[4];
    dist = 0;
    float u;
    float v;
    __m128 di =_mm_set_ps1(INF);
    __m128 org4[3], dir4[3];
    for (int i = 0; i < 3; i++) {
        org4[i] = _mm_set_ps(rays[0].getO().get(i), rays[1].getO().get(i), rays[2].getO().get(i), rays[3].getO().get(i));
        dir4[i] = _mm_set_ps(rays[0].getD().get(i), rays[1].getD().get(i), rays[2].getD().get(i), rays[3].getD().get(i));
    }
    scene->intersect(org4, dir4, di, prim);
    __m128 norm4[3];
    Vector3D colsr[4];
    for (int i = 0; i < 4; i++) {
        if (prim[i] != nullptr)
            colsr[i] = prim[i]->getNormal();
        else colsr[i] = Vector3D(0, 1, 0);
    }
    for (int i = 0; i < 3; i++) {
        norm4[i] = _mm_set_ps(colsr[0].get(i), colsr[1].get(i), colsr[2].get(i), colsr[3].get(i));
        org4[i] = _mm_add_ps(org4[i], _mm_mul_ps(_mm_sub_ps(di, _mm_set_ps1(EPS)), dir4[i]));
    }
    scene->accumulateLight(org4, dir4, norm4, colsr);
    for (int i = 0; i < 4; i++)
    {
        if (!prim[i]){ cols[i] = V3D_GREEN; continue; }
        if (prim[i]->getMat()->isEmitter()) {
            cols[i] = prim[i]->getMat()->getColor();
            continue;
        }
        cols[i] = prim[i]->getMat()->getColor() * colsr[i];
    }
}

void RayCaster::invalidateAvgs() {
    sampleNo = 0;
}