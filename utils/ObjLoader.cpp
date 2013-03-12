#include "./ObjLoader.h"
using namespace std;

void ObjLoader::Load(const char filename[], vector<Triangle*> & tris,
                     vector<Material*> & materials, Material* defMaterial,
                     Vector3D & pos,
                     Vector3D & scale,
                     bool smoothNormals) {
    LogDefault->line();
    LogDefault->outString("Loading object ./");
    LogDefault->outStringN(filename);
    LogDefault->outValue("Position", pos);
    LogDefault->outValue("scale", scale);
    LogDefault->outValue("smooth", smoothNormals);

    // the file loads, awesome!

    vector<Vertex*> vs;
    vector<Vector3D> vn;
    vn.push_back(V3D_BLANK);
    vs.push_back(0);
    char cmd[256];
    Vector3D tmpVect;
    string curMat = "";
    int a, b, c;
    int triscnt = 0;
    map<string, Material*> mats;
    bool wantScaling = scale.sqlength() > EPS;
    LogDefault->outValue("Scaling",wantScaling);
    Vector3D minVertex = V3D_BLANK;
    {
        
    ifstream obj(filename);
    if (!obj.good()) {
        LogDefault->outStringN("Loading failed, aborting");
        LogDefault->line();
        return;
    }
    while (obj >> cmd) {
        if(strcmp(cmd, "v") == 0) { //vertex
            obj >> tmpVect;
            if (wantScaling)
                tmpVect *= scale;
            minVertex += tmpVect;
            vs.push_back(
                new Vertex(tmpVect)
            );
        }
        else if (strcmp(cmd, "vn") == 0) {
            obj >> tmpVect;
            vn.push_back(tmpVect);
        }
        obj.ignore(256,'\n');
    }
    minVertex /= vs.size()-1;
    minVertex -= pos;
    LogDefault->criticalOutValue("avgVertex",minVertex);
    for (int i = vs.size()-1; i > 0; i--)
        vs[i]->setPos(vs[i]->getPos() - minVertex);
    }
    ifstream obj(filename);
    while(obj >> cmd){
        if(strcmp(cmd, "usemtl") == 0) { // material change
            obj >> cmd;
            curMat = cmd;
        }
        else if(strcmp(cmd, "f") == 0) { // triangle
            Vertex * vert[3];
            bool normals = false;
            for (int i = 0; i < 3; i++) {
                vert[i] = getVertex(obj, vs, vn, normals, smoothNormals);
            }
            tris.push_back( new Triangle(vert[0], vert[1], vert[2], !normals) );
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
Vertex * ObjLoader::getVertex(istream & in, vector<Vertex *> &vs,
                              vector<Vector3D> & vn, bool & normals,
                              bool &smoothNormals) {
    int a, b, c;
    char tmp;
    in >> a; // vertex number
    if (in.peek() != '/') {
        normals = false;
        if (smoothNormals)
            return vs[a];
        else return new Vertex(*vs[a]);
    }
    in >> tmp;
    if (in.peek() != '/' ) { // should be uv coordinates
        in >> b;
        if (in.peek() != '/') {
            Vertex * retval = new Vertex(*vs[a]);
            normals = true;
            smoothNormals = false;
            retval->setNormal(vn[b]);
            return retval;
        }
    }
    else b = 0;
    in >> tmp; // normal
    in >> c;
    normals = true;
    smoothNormals = false;
    Vertex * retval = new Vertex(*vs[a]);
    retval->setNormal(vn[c]);
    return retval;
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