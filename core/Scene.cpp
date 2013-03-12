#include "./Scene.h"

Scene::Scene(Config& config) {
    LogDefault->criticalOutValue("ScenePath", config.getStringByName("scene"));
    loadScene(config.getStringByName("scene").c_str());
    tree.Load(triangles);
}

Scene::~Scene() {
    reset();
}

void Scene::reset() {
    for (std::vector<Triangle*>::iterator it = triangles.begin();
        it != triangles.end(); it++)
        delete *it;
    for (std::vector<Material*>::iterator it = materials.begin();
        it != materials.end(); it++)
        delete *it;
    triangles.clear();
    materials.clear();
}

Triangle* Scene::intersect(Ray& ray, float& dist) {
    dist = INF;
    return tree.intersect(ray,dist);
    /*
     Triangle * retval = 0;
    for (std::vector<Triangle*>::iterator it = triangles.begin();
        it != triangles.end(); it++) {
            float tmpDist = INF;
            if ((*it)->intersect(ray, tmpDist) == Triangle::HIT) {
                if (tmpDist < dist) {
                    dist = tmpDist;
                    retval = *it;
                }
            }
    }
    return retval;*/
}

Triangle* Scene::intersect(Ray& ray, float& dist, float& u, float& v) {
    dist = INF;
   
    return tree.intersect(ray,dist,u,v);
    /*
     Triangle * retval = 0;
    float tmpu = 0;
    float tmpv = 0;
    for (std::vector<Triangle*>::iterator it = triangles.begin();
        it != triangles.end(); it++) {
            float tmpDist = 0;
            if ((*it)->intersect(ray, tmpDist, tmpu, tmpv) == Triangle::HIT) {
                if (tmpDist < dist) {
                    dist = tmpDist;
                    retval = *it;
                    u = tmpu;
                    v = tmpv;
                }
            }
    }
    return retval;
    */
}


Vector3D Scene::accumulateLight(Ray& ray, Vector3D & normal) {
    Vector3D retval = V3D_BLANK;
    Ray light(ray.origin - EPS * ray.direction,V3D_BLANK);
    for (std::vector<Triangle*>::iterator it = lights.begin();
        it != lights.end(); it++) {
            
            light.direction = (*it)->sampleSurface() - light.origin;
            float tmpDist = INF;
            intersect(light, tmpDist);
            if (tmpDist > 1.0f - EPS)
            {
                // it is very likely that it was not obscured
                // N.D/r^2, D' = sqrt(r)*D, 
                retval += -normal.dot(light.direction)/
                    (light.direction.length() * light.direction.sqlength() ) *
                    (*it)->getMat()->getColor();
            }
    }
    return retval;
}

void Scene::loadScene(const char filename[]) {
    LogDefault->line();
    LogDefault->outString("Trying to load scene from file ");
    LogDefault->outString(filename);
    LogDefault->outString("\n");
    LogDefault->flush();
    ifstream in(filename);
    if (!in.good()) {
        LogDefault->criticalOutValue("Loading failed, file not found?","");
        LogDefault->line();
        return;
    }

    char cmd[256];
    map <string, Material*> mats;
    while (in >> cmd) {   
        in.ignore(256,'\n');
        if (strcmp(cmd,"camera") == 0) 
            LoadCamera(in);
        else if (strcmp(cmd, "material") == 0)
            LoadMaterial(in, mats);
        else if (strcmp(cmd, "triangle") == 0)
            LoadTriangle(in, mats);
        else if (strcmp(cmd, "object") == 0)
            LoadObject(in, mats);
    }
    for (vector<Triangle*>::iterator it = triangles.begin();
        it != triangles.end(); it++) {
            if ((*it)->getMat()->isEmitter()) {
                lights.push_back(*it);
            }
            (*it)->normalize();
    }

}

Camera * Scene::getCamera() {
    return &cam;
}

void Scene::LoadCamera(istream & in) {
    // there are two ways to load a camera
    // either by using lookat container
    // or just supplying all the components
    char cmd[256];
    map<string, Vector3D> args;
    // not a very safe way to do this, as some arguments
    // might be presented in wrong way
    args["up"] = V3D_POINT[1];
    args["front"] = V3D_POINT[2];
    args["right"] = V3D_POINT[0];
    while (in >> cmd) {
        if (cmd[0] == '}') {
            in.ignore(256,'\n');
            break;
        }
        else if (cmd[0] == '#') {
            in.ignore(256,'\n');
            continue;
        }
        in >> args[cmd];
        in.ignore(256,'\n');
    }
    if (args.count("lookat")) //lookAt camera
        cam.set(args["pos"],args["lookat"], args["up"]);
    else cam.set(args["pos"], args["up"], args["right"], args["front"]);
}

void Scene::LoadMaterial(istream & in, map<string, Material*> & mats) {
    char cmd[256];
    Material * mat = new Material;
    materials.push_back(mat);
    Vector3D tmpV;
    while (in >> cmd) {
        if (cmd[0] == '}') {
            in.ignore(256,'\n');
            return;
        }
        else if (cmd[0] == '#') {
            in.ignore(256,'\n');
            continue;
        }
        if (strcmp(cmd,"emitter") == 0) {
            mat->setEmitter(true);
        }
        else if (strcmp(cmd, "Kd") == 0) {
            in >> tmpV;
            mat->setDifColor(tmpV);
        }
        else if (strcmp(cmd, "Ks") == 0) {
            in >> tmpV;
            // mat->SetSpecColor(tmpV);
        }
        else if (strcmp(cmd, "name") == 0) {
            in >> cmd;
            mats[cmd] = mat;
        }
        else if (strcmp(cmd, "Ns") == 0) {
            in >> tmpV[0];
            // mat->setN(tmpV[0]);
        }
        else if (strcmp(cmd, "Ni") == 0) {
            in >> tmpV[0];
            // mat->setRho(tmpV[0]);
        }
        in.ignore(256,'\n');
    }
}

void Scene::LoadTriangle(istream & in, map<string, Material *> &mats) {
    char cmd[256];
    Material * mat = 0;
    vector<Vector3D> vs;
    Vector3D tmp;
    while (in >> cmd) {
        if (cmd[0] == '}') {
            in.ignore(256,'\n');
            break;
        }
        else if (cmd[0] == '#') {
            in.ignore(256,'\n');
            continue;
        }
        if (strcmp(cmd, "material") == 0) {
            in >> cmd;
            mat = getMaterial(cmd, mats);
        }
        else{ // should all be only vectors
            in >> tmp;
            vs.push_back(tmp);
        }
        in.ignore(256, '\n');
    }
    if (vs.size() >= 3) {
        triangles.push_back(new Triangle(
            new Vertex(vs[0]),
            new Vertex(vs[1]),
            new Vertex(vs[2])));
        triangles.back()->setMat(mat);
    }
    else LogDefault->outStringN("Failed to load triangle");
}

void Scene::LoadObject(istream & in, map<string, Material *> &mats) {
    map<string, Vector3D> args;
    args["scale"] = V3D_BLANK;
    args["angle"] = V3D_BLANK;
    args["pos"] = V3D_BLANK;
    char filename[256];
    bool smoothNormals = false;
    Material * defMat = 0;

    char cmd[256];
    while (in >> cmd) {
        if (cmd[0] == '}') {
            in.ignore(256,'\n');
            break;
        }
        else if (cmd[0] == '#') {
            in.ignore(256,'\n');
            continue;
        }
        if (strcmp(cmd, "file") == 0) {
            in >> filename;
        }
        else if (strcmp(cmd, "material") == 0) {
            in >> cmd;
            defMat = getMaterial(cmd, mats);
        }
        else if (strcmp(cmd, "smoothNormals") == 0) {
            smoothNormals = true;
        }
        else {
            in >> args[cmd];
        }
        in.ignore(256,'\n');
    }
    args["rotate"].degToRad();
    ObjLoader::Load(filename, triangles, materials, defMat,
        args["pos"], args["scale"], args["rotate"], smoothNormals);
}

Material * Scene::getMaterial(const string & name, map<string, Material*> & mats) {
    if (mats.count(name))
        return mats[name];
    return 0;
}