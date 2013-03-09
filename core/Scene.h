#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "../utils/util_pack.h"
#include "../utils/Config.h"
#include "../utils/ObjLoader.h"
#include "../primitives/Triangle.h"
#include "../core/Camera.h"
class Scene {
public:
    Scene(Config& config);
    Triangle* intersect(Ray& ray, float& dist, float& u, float& v);
    Triangle* intersect(Ray& ray, float& dist);
    void loadScene(const char filename[]);
    ~Scene();
    Vector3D accumulateLight(Ray& ray, Vector3D & normal);
    Camera * getCamera();
private:
    void reset();
    std::vector<Triangle*> triangles;
    std::vector<Triangle*> lights;
    std::vector<Material*> materials;
    Camera cam;
};

#endif