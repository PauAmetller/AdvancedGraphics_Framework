#ifndef HIISHADER_H
#define HIISHADER_H

#include "shader.h"
#include "../core/hemisphericalsampler.h"


class HIIShader : public Shader
{
public:
    HIIShader();
    HIIShader(Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Vector3D Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const; //Gets the color for the point of the specular material

    Vector3D ComputeRadiance(const Ray& r, const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList, int MAX_DEPH) const;

    HemisphericalSampler HS = HemisphericalSampler();

};

#endif // HIISHADER_H