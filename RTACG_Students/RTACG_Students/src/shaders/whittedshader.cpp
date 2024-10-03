#include "whittedshader.h"
#include "../core/utils.h"


WhittedIntegrator::WhittedIntegrator() :
    Shader()
{ }

WhittedIntegrator::WhittedIntegrator(Vector3D bgColor_) :
    Shader(bgColor_)
{ }

Vector3D WhittedIntegrator::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    //(FILL..)
    Intersection its;
    Vector3D color = bgColor;
    if (Utils::getClosestIntersection(r, objList, its))
    {
        if (its.shape->getMaterial().hasSpecular()) {
            Vector3D wr = its.normal.operator*(dot(-r.d, its.normal) * 2.0) - (-r.d);
            Ray reflectionRay = Ray(its.itsPoint, wr);
            color = computeColor(reflectionRay, objList, lsList);
        }
        else {
            for (LightSource* light : lsList)
            {
                PointLightSource* PointLIght = (PointLightSource*)light;
                Vector3D position = PointLIght->sampleLightPosition();
                Vector3D directionShadowRay = position.operator-(its.itsPoint);
                Vector3D normalizeddirection = directionShadowRay.normalized();
                Ray* LightRay = new Ray(its.itsPoint, normalizeddirection, 0.0, Epsilon, directionShadowRay.length() - Epsilon);
                Intersection itsLight;
                if (!Utils::getClosestIntersection(*LightRay, objList, itsLight))
                {
                    /*If the phisics where correct the light would be reduced with the distance, which is the commented incident light,
                    but so that it looks like the figure 7 in the assigment we put it commented*/
                    //Vector3D Incident_light = light->getIntensity() * 10.0 / pow(directionShadowRay.length(), 2);
                    //This is following what the incident light would be to get the same results as figure 7
                    Vector3D Incident_light = light->getIntensity();
                    color += Incident_light * its.shape->getMaterial().getReflectance(its.normal, -r.d, normalizeddirection) * dot(normalizeddirection, its.normal);
                }
            }
            color += ambient_light * its.shape->getMaterial().getDiffuseReflectance();
        }
    }

    return color;
}
