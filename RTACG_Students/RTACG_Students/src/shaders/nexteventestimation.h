#ifndef NEESHADER_H
#define NEESHADER_H

#include "shader.h"
#include "../core/hemisphericalsampler.h"


class NEEShader : public Shader
{
public:
    NEEShader();
    NEEShader(Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Vector3D Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const; //Gets the color for the point of the specular material

    Vector3D ComputeRadiance(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList, int depth) const;
    Vector3D ReflectedRadiance(const Intersection x, const Vector3D wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList, int depth) const;
    Vector3D ComputeDirectRadiance(const Intersection x, const Vector3D wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const;
    Vector3D ComputeIndirectRadiance(const Intersection x, const Vector3D wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList, int depth) const;


    HemisphericalSampler HS = HemisphericalSampler();

};

#endif // NEESHADER_H
