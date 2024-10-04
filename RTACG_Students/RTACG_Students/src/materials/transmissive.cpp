#include "transmissive.h"

#include <iostream>

Transmissive::Transmissive()
{}

Transmissive::Transmissive(double IoR_):
IoR(IoR_) 
{}

Vector3D Transmissive::getReflectance(const Vector3D& n, const Vector3D& wo,
    const Vector3D& wi) const {
    return  Vector3D(0.0);
};

double Transmissive::getIndexOfRefraction() const
{
    return IoR;
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

    return wr.normalized();
}

Vector3D Transmissive::ComputeTransmissionDirection(const Vector3D& n, const Vector3D& wo, const bool& inside) const
{
    double IoR_ = inside ? 1.0 / getIndexOfRefraction() : getIndexOfRefraction() / 1.0;

    double dot_n_wo = dot(n, wo);
    double s = pow(IoR_, 2.0) * (1.0 - pow(dot_n_wo, 2.0));

    if (s > 1.0) {
        return Vector3D(0.0, 0.0, 0.0);
    }

    double squared_value = sqrt( 1.0 - s);

    Vector3D wt = n.operator*((IoR_ * dot_n_wo) - squared_value) - wo.operator*(IoR_);

    return wt.normalized();
}