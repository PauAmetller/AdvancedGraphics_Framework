#include "hemisphericaldirectilluminationshader.h"

#include "../core/utils.h"

bool sample_incompleted_HDI;

HDIShader::HDIShader() :
    Shader()
{ }

HDIShader::HDIShader(Vector3D bgColor_) :
    Shader(bgColor_)
{ }

Vector3D HDIShader::Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {
    Vector3D wr = its.shape->getMaterial().ComputeReflectionDirection(its.normal, -r.d);
    Ray reflectionRay = Ray(its.itsPoint, wr.normalized(), r.depth + 1.0);
    return computeColor(reflectionRay, objList, lsList);
}



Vector3D HDIShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    //(FILL..)
    if (r.depth == 0) {
        sample_incompleted_HDI = true;
    }
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

            Vector3D direct_light = Vector3D(0.0);
            Vector3D ambient = Vector3D(0.0);
            if (!material.isEmissive() && sample_incompleted_HDI) {
                sample_incompleted_HDI = false;
                for (int i = 0; i < Number_Samples; i++) {
                    Vector3D new_direction = HS.getSample(its.normal);
                    Ray new_ray = Ray(its.itsPoint, new_direction, r.depth + 1);
                    Vector3D incoming_light_color = computeColor(new_ray, objList, lsList);
                    direct_light += material.getReflectance(its.normal, -r.d, new_ray.d) * dot(new_ray.d, its.normal) * incoming_light_color * (2 * PHI);
                }
                ambient = ambient_light;
            }
            Vector3D emitted_radiance = material.getEmissiveRadiance();
            color += emitted_radiance + direct_light / Number_Samples + ambient * material.getDiffuseReflectance();
        } 
        if (sample_incompleted_HDI){
            if (material.hasSpecular()) {
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

    }

    return color;
}

