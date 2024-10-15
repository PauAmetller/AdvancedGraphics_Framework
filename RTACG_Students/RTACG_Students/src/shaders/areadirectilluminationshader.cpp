#include "areadirectilluminationshader.h"

#include "../core/utils.h"


ADIShader::ADIShader() :
    Shader()
{ }

ADIShader::ADIShader(Vector3D bgColor_) :
    Shader(bgColor_)
{ }

Vector3D ADIShader::Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {
    Vector3D wr = its.shape->getMaterial().ComputeReflectionDirection(its.normal, -r.d);
    Ray reflectionRay = Ray(its.itsPoint, wr.normalized(), r.depth + 1.0);
    return computeColor(reflectionRay, objList, lsList);
}



Vector3D ADIShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    //(FILL..)
    Intersection its;
    Vector3D color = bgColor;
    int Number_Samples = 256;

    if (r.depth > 20) {
        return color;
    }
    if (Utils::getClosestIntersection(r, objList, its))
    {
        const Material& material = its.shape->getMaterial();

        if (material.hasDiffuseOrGlossy() || material.isEmissive()) {

            Vector3D emitted_radiance = Vector3D(0.0);
            Vector3D direct_light = Vector3D(0.0);
            Vector3D ambient = Vector3D(0.0);
            if (!material.isEmissive()) {

                for (LightSource* light : lsList)
                {
                    for (int i = 0; i < Number_Samples; i++) {
                        AreaLightSource* AreaLight = (AreaLightSource*)light;
                        Vector3D position = AreaLight->sampleLightPosition();
                        Vector3D directionShadowRay = position.operator-(its.itsPoint);
                        Vector3D normalizeddirection = directionShadowRay.normalized();
                        Ray* ShadowRay = new Ray(its.itsPoint, normalizeddirection, r.depth + 1, Epsilon, directionShadowRay.length() - Epsilon);
                        Intersection itsLight;
                        if (!Utils::getClosestIntersection(*ShadowRay, objList, itsLight))
                        {
                            Vector3D Incident_light = light->getIntensity();
                            Vector3D geometric_term = dot(normalizeddirection, its.normal) * dot(-normalizeddirection, AreaLight->getNormal()) / pow(directionShadowRay.length(), 2.0);
                            direct_light += Incident_light * material.getReflectance(its.normal, -r.d, normalizeddirection) * geometric_term * AreaLight->getArea();
                        }
                    }
                    ambient = ambient_light;
                }
            }
            else {
                emitted_radiance = material.getEmissiveRadiance();
            }
            color += emitted_radiance + direct_light / Number_Samples + ambient * material.getDiffuseReflectance();
        } 
        else if (material.hasSpecular()) {
            color = Specular_ReflexionColor(its, r, objList, lsList);
        }
        else if (material.hasTransmission()) {
            Vector3D wt;
            if (dot(its.normal, -r.d) < 0) {
                wt = material.ComputeTransmissionDirection(-its.normal, -r.d, true);
            }
            else {
                wt = material.ComputeTransmissionDirection(its.normal, -r.d, false);
            }

            //Check if there's Total internal reflection
            if (wt.length() != 0.0)
            {
                Ray refractRay = Ray(its.itsPoint, wt.normalized(), r.depth + 1.0);
                color = computeColor(refractRay, objList, lsList);
            }
            else
            {
                color = Specular_ReflexionColor(its, r, objList, lsList); //In case Total internal reflection.
            }
        }

    }

    return color;
}

