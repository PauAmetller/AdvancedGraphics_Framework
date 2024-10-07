#include "whittedshader.h"
#include "../core/utils.h"


WhittedIntegrator::WhittedIntegrator() :
    Shader()
{ }

WhittedIntegrator::WhittedIntegrator(Vector3D bgColor_) :
    Shader(bgColor_)
{ }

Vector3D WhittedIntegrator::Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const{
    Vector3D wr = its.shape->getMaterial().ComputeReflectionDirection(its.normal, -r.d);
    Ray reflectionRay = Ray(its.itsPoint, wr.normalized(), r.depth + 1);
    return computeColor(reflectionRay, objList, lsList);
}



Vector3D WhittedIntegrator::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    //(FILL..)
    Intersection its;
    Vector3D color = bgColor;

    if (r.depth > 20) {
        return color;
    }
    if (Utils::getClosestIntersection(r, objList, its))
    {
        const Material& material = its.shape->getMaterial();

        if (material.hasSpecular()) {
            color = Specular_ReflexionColor(its, r, objList, lsList);
        }
        else if (material.hasTransmission()) {
            Vector3D wt;
            if (dot(its.normal , -r.d) < 0) {
                wt = material.ComputeTransmissionDirection(-its.normal , -r.d, true);
            }
            else {
                wt = material.ComputeTransmissionDirection(its.normal, -r.d, false);
            }

            //Check if there's Total internal reflection
            if (wt.length() != 0.0)
            {
                Ray refractRay = Ray(its.itsPoint, wt.normalized(), r.depth + 1);
                color = computeColor(refractRay, objList, lsList);
            }
            else
            {
                color = Specular_ReflexionColor(its, r, objList, lsList); //In case Total internal reflection.
            }
        }
        else if (material.hasDiffuseOrGlossy()) {
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
                    /*Option_1; If the phisics where correct the light would be reduced with the distance, which is the commented incident light,
                    but so that it looks like the figure 7 in the assigment we put it commented*/
                    //Vector3D Incident_light = light->getIntensity() * 10.0 / pow(directionShadowRay.length(), 2);
                    
                    //Option_2; This is following what the incident light would be to get the same results as figure 7
                    Vector3D Incident_light = light->getIntensity();
                    color += Incident_light * material.getReflectance(its.normal, -r.d, normalizeddirection) * dot(normalizeddirection, its.normal);
                }
            }
            color += ambient_light * material.getDiffuseReflectance();
        }
    }

    return color;
}
