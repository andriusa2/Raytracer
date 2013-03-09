#ifndef BXDF_H
#define BXDF_H

#include "../utils/util_pack.h"

class BxDF {
public:
    virtual float transm(const Vector3D& in,
        const Vector3D& normal,
        const Vector3D& out) = 0;
    virtual float pdf(const Vector3D& in,
        const Vector3D& normal,
        const Vector3D& out) = 0;
    virtual Vector3D getOut(const Vector3D& in,
        const Vector3D& normal,
        float & outPdf) = 0;
};

#endif