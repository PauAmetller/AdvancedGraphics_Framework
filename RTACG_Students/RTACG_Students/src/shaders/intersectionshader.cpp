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
    Intersection its;
    Vector3D color = bgColor;
    if (Utils::getClosestIntersection(r, objList, its))
    {
        for (LightSource* light : lsList) {
            PointLightSource* PointLIght = (PointLightSource*)light;
            Vector3D position = PointLIght->sampleLightPosition();
            Vector3D directionShadowRay = position.operator-(its.itsPoint);
            Ray* LightRay = new Ray(its.itsPoint, directionShadowRay.normalized(), 0.0, Epsilon, -directionShadowRay.length() - Epsilon);
            Intersection itsLight;
            if (!Utils::getClosestIntersection(*LightRay, objList, itsLight))
            {
                Vector3D Incident_light = its.shape->getMaterial().getDiffuseReflectance() * (light->getIntensity() / pow(directionShadowRay.length(), 2));
                color += Incident_light * its.shape->getMaterial().getReflectance(its.normal/*should be the normal with the dhadowray*/, -directionShadowRay, r.d);
            }
        }
        //color += ambient_light
    }

    return color;

}