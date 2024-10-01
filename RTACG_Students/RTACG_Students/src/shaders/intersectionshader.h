#ifndef INTERSECTIONSHADER_H
#define INTERSECTIONSHADER_H

#include "shader.h"

class IntersectionShader : public Shader
{
public:
    IntersectionShader();
    IntersectionShader(Vector3D hitColor, Vector3D bgColor_);

    Vector3D computeColor(const Ray &r,
                             const std::vector<Shape*> &objList,
                             const std::vector<LightSource*> &lsList) const;

    Vector3D hitColor;
};

#endif // INTERSECTIONSHADER_H

#ifndef NORMALSHADER_H
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

#endif // NORMALSHADER_H

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