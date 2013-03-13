#include "./ObjLoader.h"
using namespace std;

void ObjLoader::Load(const char filename[], vector<Triangle*> & tris,
                     vector<Material*> & materials, Material* defMaterial,
                     Vector3D & pos,
                     Vector3D & scale,
                     Vector3D & rotate,
                     bool smoothNormals) {
    LogDefault->line();
    LogDefault->outString("Loading object ./");
    LogDefault->outStringN(filename);
    LogDefault->outValue("Position", pos);
    LogDefault->outValue("scale", scale);
    LogDefault->outValue("rotate",rotate);
    LogDefault->outValue("smooth", smoothNormals);
    stringstream obj;
    ifstream in(filename);
    if (!in.good()) {
        LogDefault->outStringN("Loading failed, aborting");
        LogDefault->line();
        return;
    }
    obj << in.rdbuf();
    in.close();

    char cmd[256];
    Vector3D tmpVect;
    string curMat = "";
    int a, b, c;
    int triscnt = 0;
    map<string, Material*> mats;
    bool wantScaling = scale.sqlength() > EPS;
    bool recalcNormals = true;
    LogDefault->outValue("Scaling",wantScaling);
    Vector3D minVertex = V3D_BLANK;
    int numVertices = 0;
    int numNormals = 0;
    int numTriangles = 0;

    while (obj >> cmd)
    {
        if (strcmp(cmd, "v") == 0) { //vertex
            numVertices++;
        }
        else if (strcmp(cmd, "vn") == 0) { // normal
            numNormals++;
        }
        else if (strcmp(cmd, "f") == 0) { // triangle (face)
            numTriangles++;
            //obj.getline(cmd, 256,'\n');
        }
        else if(strcmp(cmd, "mtllib") == 0) {
            // mtl file
            obj >> cmd;
            LoadMTL(cmd, mats, materials);
        }
        obj.ignore(256,'\n');
    }
    if (numNormals > 0) {
        smoothNormals = false;
        recalcNormals = false;
    }
    Vector3D * vs;
    Vertex * verts = 0;
    vs = new Vector3D[numVertices];
    if (smoothNormals){
        verts = new Vertex[numVertices];
    }
    else
        verts = new Vertex[numTriangles * 3];
    int vID = 0;
    int tID = 0;
    int vnID = 0;
    vector<Vector3D> vn;
    vn.reserve(numNormals);
    Triangle * tri = new Triangle [numTriangles];
    // will most likely cause a reallocation
    tris.reserve(tris.size() + numTriangles);
    obj.clear();
    obj.seekg(0, ios::beg);
    if (!obj.good()){
        LogDefault->outStringN("Something is wrong, obj isn't good");
        obj.clear();
    }

    // the file loads, awesome!

    while (obj >> cmd) {
        if(strcmp(cmd, "v") == 0) { //vertex
            obj >> vs[vID];
            minVertex += vs[vID++];
        }
        else if (strcmp(cmd, "vn") == 0) {
            obj >> tmpVect;
            vn.push_back(tmpVect);
        }
        obj.ignore(256,'\n');
    }
    obj.clear();
    obj.seekg(0, ios::beg);
    if (!obj.good()){
        LogDefault->outStringN("Something is wrong, obj isn't good");
        obj.clear();
    }
    minVertex /= numVertices;
    //minVertex -= pos;
    LogDefault->criticalOutValue("avgVertex",minVertex);
    for (int i = 0; i < numVertices; i++){
        // reset to 0;0 origin (center of model)
        vs[i] -= minVertex;
        // scale it a little bit
        if (wantScaling)
            vs[i] *= scale;
        // rotate it around all the axes
        for (int axis = 0; axis < 3; axis++)
            vs[i] = Quaternion::rotate(V3D_POINT[axis],
            vs[i], rotate[axis]);
        // move the origin to `pos`
        vs[i] += pos;
    }

    while(obj >> cmd){
        if(strcmp(cmd, "usemtl") == 0) { // material change
            obj >> cmd;
            curMat = cmd;
        }
        else if(strcmp(cmd, "f") == 0) { // triangle
            Vertex * vert[3];
            for (int i = 0; i < 3; i++) {
                tri[tID].setVertex(i, 
                    getVertex(
                    3*tID+i, obj, vs, verts, vn, recalcNormals, smoothNormals
                    )
                );
            }
            tris.push_back( tri + (tID) );
            tID++;
            tris.back()->setMat(getMat(curMat, mats, defMaterial));
            tris.back()->recalc(recalcNormals);
            triscnt++;
        }
        obj.ignore(256,'\n');
    }
    if (smoothNormals)
        LogDefault->outValue("Vertices loaded", numVertices);
    else LogDefault->outValue("Vertices loaded", numTriangles*3);
    LogDefault->outValue("Triangles loaded",triscnt);
    LogDefault->line();

    // everything is autodestroyed at the end of scope
    // we want to keep our beloved vertices though
}
Vertex * ObjLoader::getVertex(int vID,
        istream & in, Vector3D vs[],
        Vertex verts[], vector<Vector3D> & vn,
        bool & recalcNormals,
        bool & smoothNormals) {
    int a, b, c;
    char tmp;
    in >> a; // vertex number
    if (in.peek() != '/') {
        recalcNormals = true;
        if (smoothNormals){
            verts[a-1].setPos(vs[a-1]);
            return verts + (a - 1);
        }
        else{
            verts[vID].setPos(vs[a-1]);
            return verts + (vID);
        }
    }
    in >> tmp;
    if (in.peek() != '/' ) { // should be uv coordinates
        in >> b;
        /*if (in.peek() != '/') {
            Vertex * retval = new Vertex(*vs[a]);
            normals = true;
            smoothNormals = false;
            retval->setNormal(vn[b]);
            return retval;
        }*/
    }
    else b = 0;
    in >> tmp; // normal
    in >> c;
    recalcNormals = false;
    Vertex * retval = 0;
    if (smoothNormals)
        retval = verts + (a - 1);
    else retval = verts + vID;
    retval->setPos(vs[a-1]);
    retval->accNormal(vn[c-1]);
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