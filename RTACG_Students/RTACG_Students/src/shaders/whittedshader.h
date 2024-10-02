#ifndef WHITTEDSHADER_H
#define WHITTEDSHADER_H

#include "shader.h"


class WhittedShader : public Shader
{
public:
    WhittedShader();
    WhittedShader(Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

};

#endif // WHITTEDSHADER_H
