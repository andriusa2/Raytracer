#include "./ObjLoader.h"
using namespace std;

void ObjLoader::Load(const char filename[], vector<Triangle*> & tris,
                     vector<Material*> & materials, Material* defMaterial,
                     bool smoothNormals) {
    LogDefault->line();
    LogDefault->outString("Loading object ./");
    LogDefault->outStringN(filename);

    ifstream obj(filename);
    if (!obj.good()) {
        LogDefault->outStringN("Loading failed, aborting");
        LogDefault->line();
        return;
    }
    // the file loads, awesome!

    vector<Vertex*> vs;
    vs.push_back(0);
    char cmd[256];
    Vector3D tmpVect;
    string curMat = "";
    int a, b, c;
    int triscnt = 0;
    map<string, Material*> mats;
    while (obj >> cmd) {
        if(strcmp(cmd, "v") == 0) { //vertex
            obj >> tmpVect;
            vs.push_back(
                new Vertex(tmpVect)
            );
        }
        else if(strcmp(cmd, "usemtl") == 0) { // material change
            obj >> cmd;
            curMat = cmd;
        }
        else if(strcmp(cmd, "f") == 0) { // triangle
            obj >> a >> b >> c;
            if (smoothNormals) 
                tris.push_back( new Triangle(vs[a], vs[b], vs[c]) );
            else tris.push_back( new Triangle(new Vertex(*vs[a]), new Vertex(*vs[b]), new Vertex(*vs[c])) );
            tris.back()->setMat(getMat(curMat, mats, defMaterial));
            triscnt++;
        }
        else if(strcmp(cmd, "mtllib") == 0){
            obj >> cmd;
            LoadMTL(cmd, mats, materials);
        }
        obj.ignore(256,'\n');
    }
    if (smoothNormals)
        LogDefault->outValue("Vertices loaded", vs.size());
    else LogDefault->outValue("Vertices loaded", triscnt*3);
    LogDefault->outValue("Triangles loaded",triscnt);
    LogDefault->line();
    if (!smoothNormals) {
        for (vector<Vertex *>::iterator it = vs.begin();
            it != vs.end(); it++) 
            if (*it){
                delete *it;
                *it = 0;
            }
    }
    // everything is autodestroyed at the end of scope
    // we want to keep our beloved vertices though
}

Material* ObjLoader::getMat(string & name,
                            map<string, Material*> & mats,
                            Material * defMaterial) {
    if (mats.count(name) == 1)
        return mats[name];
    else return defMaterial;
}

void ObjLoader::LoadMTL(const char filename[],
                        map<string, Material*> & mats,
                        vector<Material*> & materials) {
    ifstream in(filename);
    LogDefault->outString("Loading MTL file ./");
    LogDefault->outStringN(filename);
    if (!in.good()) {
        LogDefault->outStringN("File failed to load, returning");
        return;
    }
    else LogDefault->outStringN("File loaded successfully");

    char cmd[256];
    Vector3D tmpVect;
    float tmpFloat;
    while (in >> cmd) {
        if (strcmp(cmd, "newmtl") == 0) {
            in >> cmd;
            materials.push_back(new Material());
            if (strstr(cmd, "light")) {
                materials.back()->setEmitter(true);
            }
            mats[cmd] = materials.back();
        }
        else if(strcmp(cmd, "Kd") == 0) {
            in >> tmpVect;
            materials.back()->setColor(tmpVect);
        }
        in.ignore(256,'\n');
    }
}