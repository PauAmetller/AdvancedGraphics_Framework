/*#ifndef NORMALSHADER_H
#define NORMALSHADER_H

#include "shader.h"


class NormalShader : public Shader
{
public:
    NormalShader();
    NormalShader(Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,
                            const std::vector<Shape*>& objList,
                            const std::vector<LightSource*>& lsList) const;

};

#endif // NORMALSHADER_H*/




//The normalShader is implemented in the intersectionshader, look why it does not work in a separated file