#ifndef MIRRORMATERIAL
#define MIRRORMATERIAL

#include "material.h"

class Mirror : public Material
{
public:
    Mirror();

    Vector3D Mirror::getReflectance(const Vector3D& n, const Vector3D& wo,
        const Vector3D& wi) const;

    bool hasSpecular() const { return true; }
    bool hasTransmission() const { return false; }
    bool hasDiffuseOrGlossy() const { return false; }
    bool isEmissive() const { return false; }

    double getIndexOfRefraction() const;
    Vector3D getEmissiveRadiance() const;
    Vector3D getDiffuseReflectance() const;
};
#endif // MATERIAL
