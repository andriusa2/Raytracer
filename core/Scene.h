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
    Triangle* intersect(Ray& ray, float& dist);
    void loadScene(const char filename[]);
    ~Scene();
    Vector3D accumulateLight(Ray& ray, Vector3D & normal);
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