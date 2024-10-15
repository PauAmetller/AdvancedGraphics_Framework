#include "hemisphericaladdedindirectillumination.h"

#include "../core/utils.h"

bool sample_incompleted_HII;

HIIShader::HIIShader() :
    Shader()
{ }

HIIShader::HIIShader(Vector3D bgColor_) :
    Shader(bgColor_)
{ }

Vector3D HIIShader::Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {
    Vector3D wr = its.shape->getMaterial().ComputeReflectionDirection(its.normal, -r.d);
    Ray reflectionRay = Ray(its.itsPoint, wr.normalized(), r.depth + 1.0);
    return computeColor(reflectionRay, objList, lsList);
}

Vector3D HIIShader::ComputeRadiance(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList, int MAX_DEPH) const 
{
    Intersection itsR;
    Vector3D radiance = Vector3D(0.0);
    if (Utils::getClosestIntersection(r, objList, itsR))
    {
        const Material& material = itsR.shape->getMaterial();
        if (material.isEmissive()) {
            return material.getEmissiveRadiance();
        }
        else if (material.hasDiffuseOrGlossy()) {
            Vector3D emitted_light = material.getEmissiveRadiance();
            radiance += emitted_light;
            Vector3D wi = HS.getSample(itsR.normal);
            if (r.depth < MAX_DEPH) {
                Ray rNew = Ray(itsR.itsPoint, wi, r.depth + 1);
                radiance += ComputeRadiance(rNew, objList, lsList, MAX_DEPH) * material.getReflectance(itsR.normal, -r.d, rNew.d) * dot(itsR.normal, wi) * (2 * PHI);
            }
        }
    }
    return radiance;
}


Vector3D HIIShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    //(FILL..)
    if (r.depth == 0) {
        sample_incompleted_HII = true;
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

        if (material.isEmissive()) {
            Vector3D emitted_radiance = material.getEmissiveRadiance();   // Check if it has to be multiplied by the diffuseRefelctance
            color += emitted_radiance;
        }
        else if (material.hasDiffuseOrGlossy()) {

            Vector3D direct_light = Vector3D(0.0);
            Vector3D indirect_light = Vector3D(0.0);
            if (!sample_incompleted_HII) {
                indirect_light += ComputeRadiance(r, objList, lsList, 4);
                color += indirect_light + direct_light;
            }
            else {
                sample_incompleted_HII = false;
                for (int i = 0; i < Number_Samples; i++) {
                    Vector3D new_direction = HS.getSample(its.normal);
                    Ray new_ray = Ray(its.itsPoint, new_direction, r.depth + 1);
                    Vector3D incoming_light_color = computeColor(new_ray, objList, lsList);
                    direct_light += material.getReflectance(its.normal, -r.d, new_ray.d) * dot(new_ray.d, its.normal) * incoming_light_color * (2 * PHI);
                    indirect_light += ComputeRadiance(r, objList, lsList, 4);
                }
                color += (indirect_light + direct_light) / Number_Samples;
            }
        }
        else if (material.hasSpecular()) {
            color = Specular_ReflexionColor(its, r, objList, lsList);
        }
        else if (material.hasTransmission()) { //&& r.depth < 1) { //The second eliminates the light thought transmissive materials
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

