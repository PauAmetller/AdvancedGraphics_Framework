#include "material.h"

#include <iostream>

Material::Material()
{ }

double Material::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;

    return -1;
}


Vector3D Material::getEmissiveRadiance() const
{
    std::cout << "Warning! Calling \"Material::getEmissiveRadiance()\" for a non-Emissive material"
        << std::endl;

    return -1;
}


Vector3D Material::getDiffuseReflectance() const
{
    std::cout << "Warning! Calling \"Material::getDiffuseReflectance()\" for a non-Diffuse material"
        << std::endl;

    return -1;
}

Vector3D Material::ComputeReflectionDirection(const Vector3D& n, const Vector3D& wo) const
{
    std::cout << "Warning! Calling \"Material::ComputeReflectionDirection()\" for a non-specular material"
        << std::endl;

    return -1;
}

Vector3D Material::ComputeTransmissionDirection(const Vector3D& n, const Vector3D& wo, const bool& inside) const
{
    std::cout << "Warning! Calling \"Material::ComputeReflectionDirection()\" for a non-transmissive material"
        << std::endl;

    return -1;
}