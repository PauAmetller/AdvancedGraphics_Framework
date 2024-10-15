#include "arealightsource.h"

AreaLightSource::AreaLightSource(Square* areaLightsource_) :
    myAreaLightsource(areaLightsource_)
{ }



Vector3D AreaLightSource::getIntensity() const
{
    return myAreaLightsource->getMaterial().getEmissiveRadiance();
}


Vector3D AreaLightSource::sampleLightPosition()   const
{
    //FILL(...)
    double fv1 = (double)std::rand() / RAND_MAX;
    double fv2 = (double)std::rand() / RAND_MAX;
    Vector3D pos = myAreaLightsource->corner + myAreaLightsource->v1 * fv1 + myAreaLightsource->v2 * fv2;
    //New Randam Pos inside Area Lightsource
    return pos;
}

