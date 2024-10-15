#include "areaaddedindirectillumination.h"

#include "../core/utils.h"

#define MAX_DEPTH 4

AIIShader::AIIShader() :
    Shader()
{ }

AIIShader::AIIShader(Vector3D bgColor_) :
    Shader(bgColor_)
{ }

Vector3D AIIShader::Specular_ReflexionColor(Intersection its, const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {
    Vector3D wr = its.shape->getMaterial().ComputeReflectionDirection(its.normal, -r.d);
    Ray reflectionRay = Ray(its.itsPoint, wr.normalized(), r.depth + 1.0);
    return computeColor(reflectionRay, objList, lsList);
}

Vector3D AIIShader::ComputeRadiance(const Ray & r, const std::vector<Shape*>&objList, const std::vector<LightSource*>&lsList, int depth) const
{
    Intersection itsR;
    Vector3D Lo = Vector3D(0.0);
    Vector3D Lr = Vector3D(0.0);

    if (Utils::getClosestIntersection(r, objList, itsR))
    {
        Lo = itsR.shape->getMaterial().getEmissiveRadiance();
        Lr = ReflectedRadiance(itsR, -r.d, objList, lsList, r.depth + 1.0);
    }
    return Lo + Lr;
}

Vector3D AIIShader::ReflectedRadiance(const Intersection x, const Vector3D wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList, int depth) const
{
    Vector3D Ldir = ComputeDirectRadiance(x, wo, objList, lsList);
    Vector3D Lind = ComputeIndirectRadiance(x, wo, objList, lsList, depth);
    return Ldir + Lind;
}


Vector3D AIIShader::ComputeDirectRadiance(const Intersection x, const Vector3D wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Vector3D Ldir = Vector3D(0.0);
    for (LightSource* light : lsList)
    {
        AreaLightSource* AreaLight = dynamic_cast<AreaLightSource*>(light);
        if (!AreaLight) {
            continue; // Skip if not an area light
        }
        Vector3D position = AreaLight->sampleLightPosition();
        Vector3D directionShadowRay = position.operator-(x.itsPoint);
        Vector3D normalizeddirection = directionShadowRay.normalized();
        Ray ShadowRay = Ray(x.itsPoint, normalizeddirection, 1, Epsilon, directionShadowRay.length() - Epsilon);

        Intersection itsLight;
        if (!Utils::getClosestIntersection(ShadowRay, objList, itsLight))
        {
            Vector3D Incident_light = light->getIntensity();
            double geometric_term = dot(normalizeddirection, x.normal) * dot(-normalizeddirection, AreaLight->getNormal()) / pow(directionShadowRay.length(), 2.0);
            Ldir += Incident_light * x.shape->getMaterial().getReflectance(x.normal, wo, normalizeddirection) * geometric_term * AreaLight->getArea();
        }
    }

    return Ldir;
}

Vector3D AIIShader::ComputeIndirectRadiance(const Intersection x, const Vector3D wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList, int depth) const
{
    Vector3D Lind = Vector3D(0.0);
    Vector3D wi = HS.getSample(x.normal);
    Intersection y;
    if (depth < MAX_DEPTH) {
       Ray rNew = Ray(x.itsPoint, wi, depth + 1);
       if (Utils::getClosestIntersection(rNew, objList, y))
       {
           Lind += ReflectedRadiance(y, -rNew.d, objList, lsList, rNew.depth) * x.shape->getMaterial().getReflectance(x.normal, wo, rNew.d) * dot(x.normal, wi) * (2 * PHI);
       }
    }
    return Lind;
}


Vector3D AIIShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
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
            Vector3D light = Vector3D(0.0);
            if (!material.isEmissive()) {
                for (int i = 0; i < Number_Samples; i++) {
                    light += ComputeRadiance(r, objList, lsList, r.depth);
                }
            }
            else {
                emitted_radiance = material.getEmissiveRadiance();
            }
            color += emitted_radiance + light / Number_Samples;
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
