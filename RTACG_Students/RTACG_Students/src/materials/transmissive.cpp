#include "transmissive.h"

#include <iostream>

Transmissive::Transmissive()
{ }

Transmissive::Transmissive(double Ut_):
Ut(Ut_) {}

Vector3D Transmissive::getReflectance(const Vector3D& n, const Vector3D& wo,
    const Vector3D& wi) const {
    return  Vector3D(0.0);
};

double Transmissive::getIndexOfRefraction() const
{
    return Ut;
}


Vector3D Transmissive::getEmissiveRadiance() const
{
    return Vector3D(0.0);
}


Vector3D Transmissive::getDiffuseReflectance() const
{
    return Vector3D(0.0);
}

Vector3D Transmissive::ComputeReflectionDirection(const Vector3D& n, const Vector3D& wo) const
{
    Vector3D wr = n.operator*(dot(wo, n) * 2.0) - wo;

    return wr;
}

Vector3D Transmissive::ComputeTransmissionDirection(const Vector3D& n, const Vector3D& wo) const
{
    double dot_n_wo = dot(n, wo);
    double square_value = 1.0 - pow(Ut, 2.0) * (1 - pow(dot_n_wo, 2.0));

    if (square_value < 0.0) {
        return Vector3D(0.0, 0.0, 0.0);
    }

    double squared_value = sqrt(square_value);

    Vector3D wt = wo.operator*(-Ut) + n.operator*(Ut * dot_n_wo - squared_value);

    return wt;
}