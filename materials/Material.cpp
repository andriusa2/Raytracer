#include "./Material.h"

Material::Material(int BRDF, Vector3D col, bool emit): color(col), type(BRDF), emits(emit)
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

bool Material::isEmitter() {
    return emits;
}

void Material::setEmitter(bool val) {
    emits = val;
}

void Material::setDifColor(Vector3D & col) {
    color = col;
}