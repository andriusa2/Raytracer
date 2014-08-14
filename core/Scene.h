#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <fstream>
#include "../utils/util_pack.h"
#include "../utils/Config.h"
#include "../utils/ObjLoader.h"
#include "../primitives/Triangle.h"
#include "../core/Camera.h"
#include "../accel/KdTree.h"
class Scene {
public:
    Scene(Config& config);
    Triangle* intersect(Ray& ray, float& dist, float& u, float& v);
    Triangle* intersect(Ray& ray, float& dist, bool perf=false);
    void intersect(__m128 org4[], __m128 dir4[], __m128 & dist, Triangle * triangles[], bool perf=false, bool early_out=false);
    void loadScene(const char filename[]);
    ~Scene();
    Vector3D accumulateLight(Ray& ray, Vector3D & normal);

    void accumulateLight(__m128 org4[], __m128 dir4[], __m128 normal4[], Vector3D color[4]);
    Camera * getCamera();
    KdTree & getTree() {return tree;}
private:
    void LoadCamera(istream & in);
    void LoadMaterial(istream & in, map<string, Material*> &mats);
    void LoadTriangle(istream & in, map<string, Material*> &mats);
    void LoadObject(istream & in, map<string, Material*> &mats);
    Material * getMaterial(const string & name, map<string, Material*> &mats);
    void reset();
    std::vector<Triangle*> triangles;
    std::vector<Triangle*> lights;
    std::vector<Material*> materials;
    KdTree tree;
    Camera cam;
};

#endif