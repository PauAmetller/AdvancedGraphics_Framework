#include "phong.h"

#include <iostream>

Phong::Phong()
{ }

Phong::Phong(Vector3D Kd_, Vector3D Ks_, float alpha_):
rho_d(Kd_), Ks(Ks_), alpha(alpha_){}


Vector3D Phong::getReflectance(const Vector3D& n, const Vector3D& wo,
    const Vector3D& wi) const {

    //FILL(...)
    Vector3D wr =  n.operator*(dot(wi, n) * 2.0) - wi;

    Vector3D color = (getDiffuseReflectance() / PHI) + Ks.operator*(2 * PHI / (alpha + 1)) * pow(dot(wo, wr), alpha);

    //where n is the normal at point x. This formulation respects the energy conservation law as long as kd+ks ? 1  (Maybe later its needed for something)
    return color;
};

double Phong::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;

    return -1;
}


Vector3D Phong::getEmissiveRadiance() const
{
    return Vector3D(0.0);
}


Vector3D Phong::getDiffuseReflectance() const
{
    return rho_d;
}

Vector3D Phong::ComputeReflectionDirection(const Vector3D& n, const Vector3D& wo) const
{
    std::cout << "Warning! Calling \"Material::ComputeReflectionDirection()\" for a non-specular material"
        << std::endl;

    return -1;
}

Vector3D Phong::ComputeTransmissionDirection(const Vector3D& n, const Vector3D& wo, const bool& inside) const
{
    std::cout << "Warning! Calling \"Material::ComputeReflectionDirection()\" for a non-transmissive material"
        << std::endl;

    return -1;
}