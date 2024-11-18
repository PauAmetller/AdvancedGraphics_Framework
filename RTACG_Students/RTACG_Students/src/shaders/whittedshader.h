#ifndef WHITTEDSHADER_H
#define WHITTEDSHADER_H

#include "shader.h"


class WhittedIntegrator : public Shader
{
public:
    WhittedIntegrator();
    WhittedIntegrator(Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Vector3D Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const; //Gets the color for the point of the specular material

    Vector3D ComputeRadiance(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList, int depth) const {return  Vector3D(0.0); };

    Vector3D ambient_light = Vector3D(0.15, 0.15, 0.15);

};

#endif // WHITTEDSHADER_H
