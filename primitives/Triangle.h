#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <ostream>

#include "../utils/Vector3D.h"
#include "../utils/Ray.h"
#include "./Vertex.h"
#include "../materials/Material.h"

using namespace std;

class Triangle {
public:
    enum{
        MISS = 0,
        HIT = 1
    };
    Triangle();
    Triangle(Vertex* v1, Vertex* v2, Vertex* v3, bool calcNorms=true);
    ~Triangle();
    Material* getMat() const;
    void setMat(Material * a);
    int intersect(Ray& ray, float& dist, float& u, float& v)const;
    int intersect(Ray& ray, float& dist)const;
    Vector3D getNormal() const;
    Vector3D getNormal(const float& u, const float& v) const;
    void normalize();
    Vertex* getVertex(int ID) const;
    void setVertex(int ID, Vertex* V);
    friend ostream& operator<< (ostream& out, const Triangle& tri);
    Vector3D sampleSurface() const;
private:
    void recalcNorms();
    void precompute();
    Vertex* vertices[3];
    Material* mat;
    int k;
    float nu, nv, nd;
    float bnu, bnv, cnu, cnv;
};

#endif