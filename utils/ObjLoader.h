#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <sstream>

#include "./util_pack.h"
#include "../materials/material_pack.h"
#include "../primitives/Triangle.h"
#include "./Quaternion.h"
class ObjLoader {
public:
    static void Load(const char filename[],
        vector<Triangle*> & triangles,
        vector<Material*> & materials,
        Material* defMaterial,
        Vector3D & pos,
        Vector3D & scale,
        Vector3D & rotate,
        bool smoothNormals = false);
private:
    static Vertex * getVertex(int vID,
        istream & in, Vector3D vs[],
        Vertex verts[], vector<Vector3D> & vn,
        bool & recalcNormals,
        bool & smoothNormals);
    static void LoadMTL(const char filename[],
        map<string, Material*> & mats,
        vector<Material*> & materials);
    static Material* getMat(string & name,
        map<string, Material*> & mats,
        Material* defMaterial);

};

#endif