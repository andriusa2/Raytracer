#include "./ObjLoader.h"

void ObjLoader::Load(const char filename[], vector<Triangle*> & tris,
                     vector<Material*> & mats, Material* defMaterial) {
    LogDefault.line();
    LogDefault.outString("Loading object ./");
    LogDefault.outString(filename);

    ifstream obj(filename);
    if (!obj.good()) {
        LogDefault.outString("Loading failed, aborting");
        LogDefault.line();
        return;
    }
    // the file loads, awesome!
    // atm assume no mtls, no normals, no uvs

    vector<Vertex*> vs;
    vs.push_back(0);
    char cmd;
    float x, y, z;
    int a,b,c;
    int triscnt = 0;
    while (obj >> cmd) {
        switch(cmd) {
        case 'v': // it's vertex
            obj >> x >> y >> z;
            vs.push_back(
                new Vertex(
                    Vector3D(x, y, z)
                )
            );
            break;
        case 'f': // it's a triangle
            obj >> a >> b >> c;
            tris.push_back( new Triangle(vs[a], vs[b], vs[c]) );
            tris.back()->setMat(defMaterial);
            triscnt++;
            break;
        }
        obj.ignore(256,'\n');
    }
    LogDefault.outValue("Vertices loaded", vs.size());
    LogDefault.outValue("Triangles loaded",triscnt);
    LogDefault.line();
    // everything is autodestroyed at the end of scope
    // we want to keep our beloved vertices though
}