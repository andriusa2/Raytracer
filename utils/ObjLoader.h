#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <vector>
#include <fstream>

#include "./util_pack.h"
#include "../materials/material_pack.h"
#include "../primitives/Triangle.h"
class ObjLoader {
public:
    static void Load(const char filename[],
        vector<Triangle*> & triangles,
        vector<Material*> & materials);
private:

};

#endif