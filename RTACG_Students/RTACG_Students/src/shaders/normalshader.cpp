#include "normalshader.h"
#include "../core/utils.h"

NormalShader::NormalShader() :
    Shader()
{ }

NormalShader::NormalShader(Vector3D bgColor_) :
    Shader(bgColor_)
{ }

Vector3D NormalShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    //(FILL..)
    Intersection its;

    if (Utils::getClosestIntersection(r, objList, its))
    {
        Vector3D color = (its.normal + Vector3D(1.0, 1.0, 1.0)) / 2.0;
        return color;
    }
    else {
        return bgColor;
    }
}
