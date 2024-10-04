#ifndef TRANSMISSIVEMATERIAL
#define TRANSMISSIVEMATERIAL

#include "material.h"

class Transmissive : public Material
{
public:
    Transmissive();
    Transmissive(double IoR_);

    Vector3D Transmissive::getReflectance(const Vector3D& n, const Vector3D& wo,
        const Vector3D& wi) const;

    bool hasSpecular() const { return false; }
    bool hasTransmission() const { return true; }
    bool hasDiffuseOrGlossy() const { return false; }
    bool isEmissive() const { return false; }

    double getIndexOfRefraction() const;
    Vector3D getEmissiveRadiance() const;
    Vector3D getDiffuseReflectance() const;
    Vector3D ComputeReflectionDirection(const Vector3D& n, const Vector3D& wo) const;
    Vector3D ComputeTransmissionDirection(const Vector3D& n, const Vector3D& wo, const bool& inside) const;

private:
    double IoR; //transmission ratio
};
#endif // MATERIAL

