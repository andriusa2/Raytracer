#include "./Material.h"

Material::Material(int BRDF, Vector3D col): color(col), type(BRDF)
{
}

Material::~Material() {
    // no dynamic objects atm
}

Vector3D& Material::getColor() {
    return color;
}

Vector3D& Material::getColor(const float& u, const float& v){
    return color;
}

int Material::getType() {
    return type;
}