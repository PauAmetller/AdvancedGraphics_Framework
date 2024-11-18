#ifndef DEPTHSHADER_H
#define DEPTHSHADER_H

#include "shader.h"


class DepthShader : public Shader
{
public:
    DepthShader();
    DepthShader(Vector3D color_, double maxDist_, Vector3D bgColor_);

    Vector3D computeColor(const Ray &r,
                             const std::vector<Shape*> &objList,
                             const std::vector<LightSource*> &lsList) const;

    Vector3D ComputeRadiance(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList, int depth) const { return  Vector3D(0.0); };

private:
    double maxDist;
    Vector3D color;
};

#endif // DEPTHSHADER_H
