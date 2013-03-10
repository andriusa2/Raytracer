#ifndef MATERIAL_H
#define MATERIAL_H

#include "../utils/util_pack.h"

class Material{
public:
    enum {
        LAMB = 1
    };
    Material():color(V3D_GREEN), type(LAMB), emits(false) {};
    Material(int BRDF, Vector3D col, bool emit = false);
    ~Material();
    Vector3D& getColor();
    Vector3D& getColor(const float& u, const float& v);
    void setColor(Vector3D & col) {setDifColor(col);}
    void setDifColor(Vector3D & col);
    int getType();
    bool isEmitter();
    void setEmitter(bool val);
private:
    int type;
    Vector3D color;
    bool emits;

};



#endif