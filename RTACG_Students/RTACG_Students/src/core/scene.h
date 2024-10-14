#ifndef SCENE_H
#define SCENE_H

#include "vector3d.h"
#include <stdlib.h> /* srand, rand */
#include <vector>
#include "../lightsources/pointlightsource.h"
#include "../lightsources/arealightsource.h"
#include "../shapes/shape.h"


// Class used to store information regarding the
// intersection point.
// Based on PBRT (Chapter 2)
class Scene
{
public:
    Scene();

    void AddObject(Shape* new_object);
    
    void AddPointLight(PointLightSource* new_pointLight);
    void AddAreaLight(AreaLightSource* new_areaLight);
                                 
    // Declare pointers to all the variables which describe the scene
    std::vector<Shape*>* objectsList;
    std::vector<LightSource*>* LightSourceList;
};

#endif 
