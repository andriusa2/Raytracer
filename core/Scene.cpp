#include "./Scene.h"

Scene::Scene(Config& config) {
    LogDefault->criticalOutValue("ScenePath", config.getStringByName("scene"));
    loadScene(config.getStringByName("scene").c_str());
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
    return retval;
}

Triangle* Scene::intersect(Ray& ray, float& dist, float& u, float& v) {
    dist = INF;
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

}


Vector3D Scene::accumulateLight(Ray& ray, Vector3D & normal) {
    Vector3D retval = V3D_BLANK;
    Ray light(ray.origin,V3D_BLANK);
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

    char cmd = 0;
    char cmd1 = 0;
    Vector3D arg0;
    Vector3D arg1;
    Vector3D arg2;
    float argf0;
    string buf0, buf1;
    bool argb0 = false;
    map <string, Material*> mats;
    while (in >> cmd) {
        
        in.ignore(256,'\n');
        switch(cmd) {
        case 'c': // camera
            while ((in >> cmd1)) {
                if (cmd1 == '}') break;
                if (cmd1 == 'p') {
                    in.ignore(256,' ');
                    in >> arg0;
                }
                if (cmd1 == 'l') {
                    in.ignore(256, ' ');
                    in >> arg1;
                }
                if (cmd1 == 't') {
                    in.ignore(256, ' ');
                    in >> arg2;
                }
                in.ignore(256,'\n');
            }
            cam.set(arg0, arg1, arg2);
            break;
        case 'm': // material
            argb0 = false;
            while ((in >> cmd1)) {
                if (cmd1 == '}') break;
                if (cmd1 == 'n') {
                    in.ignore(256, ' ');
                    in >> buf0;
                }
                if (cmd1 == 'c') {
                    in.ignore(256, ' ');
                    in >> arg0;
                }
                if (cmd1 == 'e') {
                    argb0 = true;
                }
                in.ignore(256,'\n');
            }
            materials.push_back(new Material(Material::LAMB, arg0, argb0));
            mats[buf0] = materials.back();
            break;
        case 'o': // object
            while (in >> cmd1) {
                if(cmd1 == '}') break;
                if (cmd1 == 'f') {
                    in.ignore(256,' ');
                    in >> buf0;
                }
                if (cmd1 == 'm') {
                    in.ignore(256,' ');
                    in >> buf1;
                }
                in.ignore(256,'\n');
            }
            LogDefault->criticalOutValue("LoadObjFile",buf0);
            ObjLoader::Load(buf0.c_str(),triangles,materials,mats[buf1]);
            break;
        case 't' :
            while (in >> cmd1) {
                if (cmd1 == '}') break;
                if (cmd1 == '0') {
                    in >> arg0;
                }
                if (cmd1 == '1') {
                    in >> arg1;
                }
                if (cmd1 == '2') {
                    in >> arg2;
                }
                if (cmd1 == 'm') {
                    in.ignore(256, ' ');
                    in >> buf0;
                }
            }
            triangles.push_back(new Triangle (
                new Vertex(arg0),
                new Vertex(arg1),
                new Vertex(arg2)));
            triangles.back()->setMat(mats[buf0]);
        default:
            in.ignore(256,'\n');
        }
    }
    for (vector<Triangle*>::iterator it = triangles.begin();
        it != triangles.end(); it++) {
            if ((*it)->getMat()->isEmitter()) {
                lights.push_back(*it);
                LogDefault->outValue("emitter",*it);
            }
            (*it)->normalize();
    }

}

Camera * Scene::getCamera() {
    return &cam;
}