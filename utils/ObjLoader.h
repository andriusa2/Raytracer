#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <vector>
#include <fstream>
#include <map>
#include <string>

#include "./util_pack.h"
#include "../materials/material_pack.h"
#include "../primitives/Triangle.h"

class ObjLoader {
public:
    static void Load(const char filename[],
        vector<Triangle*> & triangles,
        vector<Material*> & materials,
        Material* defMaterial,
        Vector3D & pos,
        Vector3D & scale,
        bool smoothNormals = false);
private:
    static void LoadMTL(const char filename[],
        map<string, Material*> & mats,
        vector<Material*> & materials);
    static Material* getMat(string & name,
        map<string, Material*> & mats,
        Material* defMaterial);

};

#endif