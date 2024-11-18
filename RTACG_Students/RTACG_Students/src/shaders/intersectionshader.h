#ifndef INTERSECTIONSHADER_H
#define INTERSECTIONSHADER_H

#include "shader.h"

class IntersectionShader : public Shader
{
public:
    IntersectionShader();
    IntersectionShader(Vector3D hitColor, Vector3D bgColor_);

    Vector3D computeColor(const Ray &r,
                             const std::vector<Shape*> &objList,
                             const std::vector<LightSource*> &lsList) const;

    Vector3D ComputeRadiance(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList, int depth) const { return  Vector3D(0.0); };

    Vector3D hitColor;
};

#endif // INTERSECTIONSHADER_H