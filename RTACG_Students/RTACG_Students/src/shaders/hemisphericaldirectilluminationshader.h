#ifndef HDISHADER_H
#define HDISHADER_H

#include "shader.h"
#include "../core/hemisphericalsampler.h"


class HDIShader : public Shader
{
public:
    HDIShader();
    HDIShader(Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Vector3D Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const; //Gets the color for the point of the specular material


    Vector3D ambient_light = Vector3D(0.15, 0.15, 0.15);
    HemisphericalSampler HS = HemisphericalSampler();

};

#endif // HDISHADER_H