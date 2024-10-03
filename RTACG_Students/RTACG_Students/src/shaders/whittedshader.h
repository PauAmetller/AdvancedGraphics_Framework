#ifndef WHITTEDSHADER_H
#define WHITTEDSHADER_H

#include "shader.h"


class WhittedIntegrator : public Shader
{
public:
    WhittedIntegrator();
    WhittedIntegrator(Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Vector3D ambient_light = Vector3D(0.15, 0.15, 0.15);

};

#endif // WHITTEDSHADER_H
