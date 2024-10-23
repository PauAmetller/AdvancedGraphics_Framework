#ifndef PPTSHADER_H
#define PPTSHADER_H

#include "shader.h"
#include "../core/hemisphericalsampler.h"


class PPTShader : public Shader
{
public:
    PPTShader();
    PPTShader(Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Vector3D Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const; //Gets the color for the point of the specular material

    Vector3D ComputeRadiance(const Ray& r, const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList, int MAX_DEPH) const;

    HemisphericalSampler HS = HemisphericalSampler();

};

#endif // PPTSHADER_H