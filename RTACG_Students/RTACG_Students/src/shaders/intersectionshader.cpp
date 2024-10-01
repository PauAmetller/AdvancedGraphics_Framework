#include "intersectionshader.h"
#include "../core/utils.h"

IntersectionShader::IntersectionShader() :
    hitColor(Vector3D(1, 0, 0))
{ }

IntersectionShader::IntersectionShader(Vector3D hitColor_, Vector3D bgColor_) :
    Shader(bgColor_), hitColor(hitColor_)
{ }

Vector3D IntersectionShader::computeColor(const Ray &r, const std::vector<Shape*> &objList, const std::vector<LightSource*> &lsList) const
{
    //(FILL..)
        
    if (Utils::hasIntersection(r, objList))
    {
        return hitColor;

    }
    else {
        return bgColor;
    }
}

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

WhittedShader::WhittedShader() :
    Shader()
{ }

WhittedShader::WhittedShader(Vector3D bgColor_) :
    Shader(bgColor_)
{ }

Vector3D WhittedShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    //(FILL..)

    return bgColor;

}