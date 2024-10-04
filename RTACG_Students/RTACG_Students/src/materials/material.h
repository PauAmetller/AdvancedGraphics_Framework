#ifndef MATERIAL
#define MATERIAL

#include "../core/vector3d.h"

#define PHI 3.14159265358979323846

class Material
{
public:
    Material();

    virtual Vector3D getReflectance(const Vector3D &n, const Vector3D &wo,
                                    const Vector3D &wi) const = 0; //Return Phong BRDF of Phong Materials and Emissive Diffuse

    virtual double getIndexOfRefraction() const; // Return Refraction ratio of Transmissive Materials
    virtual Vector3D getEmissiveRadiance() const; //Return Emissive Radiance of Emissive Materials
    virtual Vector3D getDiffuseReflectance() const; //Return Difusse Coefficient of Phong Materials
    virtual Vector3D ComputeReflectionDirection(const Vector3D& n, const Vector3D& wo) const; //Return direction vector from reflexion in specular materials
    virtual Vector3D ComputeTransmissionDirection(const Vector3D& n, const Vector3D& wo, const bool& inside) const;

    //Functions to check the material of the object
    virtual bool hasSpecular() const = 0;
    virtual bool hasTransmission() const = 0;
    virtual bool hasDiffuseOrGlossy() const = 0;
    virtual bool isEmissive() const = 0;
    
};


#endif // MATERIAL
