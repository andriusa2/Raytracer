#ifndef MATERIAL_H
#define MATERIAL_H

#include "../utils/util_pack.h"

class Material{
public:
    enum {
        LAMB = 1
    };

    Material(int BRDF, Vector3D col);
    ~Material();
    Vector3D& getColor();
    Vector3D& getColor(const float& u, const float& v);
    int getType();
private:
    int type;
    Vector3D color;


};



#endif