#ifndef CACHING_H
#define CACHING_H

#pragma once
#include <iostream>
#include <algorithm>

#include "../core/film.h"
#include "../core/matrix4x4.h"
#include "../core/ray.h"
#include "../core/utils.h"
#include "../core/scene.h"


#include "../shapes/sphere.h"
#include "../shapes/infiniteplan.h"

#include "../cameras/ortographic.h"
#include "../cameras/perspective.h"

#include "../shaders/intersectionshader.h"
#include "../shaders/depthshader.h"
#include "../shaders/normalshader.h"
#include "../shaders/whittedshader.h"
#include "../shaders/hemisphericaldirectilluminationshader.h"
#include "../shaders/areadirectilluminationshader.h"
#include "../shaders/purepathtracing.h"
#include "../shaders/nexteventestimation.h"


#include "../materials/phong.h"
#include "../materials/emissive.h"
#include "../materials/mirror.h"
#include "../materials/transmissive.h"

#include <chrono>

class RendererCaching
{
private:
    // Octree Node
    struct OctreeNode {
        Vector3D center;  // Center of this node
        double size;     // Length of one edge of the cube //We assime the hight and width resolution are the same
        Intersection its;
        double col, lin;
        Vector3D irradiance;  // Cached irradiance at this node
        Vector3D gradient;   // Gradient at this node
        bool isLeaf;        // True if this node is a leaf
        //Vector3D rotGrad, transGrad;
        std::vector<std::shared_ptr<OctreeNode>> children;  // 4 children nodes

        OctreeNode(const Vector3D& c, double s, Intersection its_, double col_, double lin_)
            : center(c), size(s), its(its_), col(col_), lin(lin_), irradiance(Vector3D(0.0)), /*rotGrad(Vector3D(0.0)), transGrad(Vector3D(0.0)),*/ gradient(Vector3D(0.0)), isLeaf(true) {}

        // Subdivide the node into 4 smaller children (based on Z axis)
        void subdivide(Camera* cam, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList, double width, double height) {
            if (isLeaf) {
                double halfSize = size / 2.0;

                if (halfSize < 3)
                    return;

                Intersection its;
                Ray cameraRay;
                std::vector<std::shared_ptr<OctreeNode>> possible_childrens;

                // Create 4 children: based on the Z axis, two nodes above and two nodes below the plane
                for (int i = 0; i < 4; ++i) {

                    double x_displacement = (i == 1 || i == 2) ? 1 : -1;
                    double y_displacement = (i == 1 || i == 3) ? 1 : -1;

                    double new_col = this->col + (halfSize * x_displacement);
                    double new_lin = this->lin + (halfSize * y_displacement);

                    double x = (double)(new_col + 0.5) / width;
                    double y = (double)(new_lin + 0.5) / height;

                    cameraRay = cam->generateRay(x, y);

                    if (Utils::getClosestIntersection(cameraRay, *objectsList, its))
                    {
                        std::shared_ptr<OctreeNode> possible_children = std::make_shared<OctreeNode>(its.itsPoint, halfSize, its, new_col, new_lin);

                        possible_childrens.push_back(possible_children);
                        getInstance()->ComputeIrradiance(possible_children.get(), cam, objectsList, lightSourceList);
                    }
                    else {
                        // Discard point if it doesn't intersect any surface
                        std::cerr << "Point (" << its.itsPoint.x << ", " << its.itsPoint.y << ", " << its.itsPoint.z
                            << ") does not intersect any surface.\n";
                    }
                }


                if (getInstance()->IrradianceGradientHigherThanThreshold(possible_childrens, objectsList)) {
                    this->isLeaf = false;
                    getInstance()->node_count--;
                    for (std::shared_ptr<OctreeNode> add_children : possible_childrens) {
                        this->children.push_back(add_children);
                        getInstance()->node_count++;
                    }
                }
            }
        }
    };

    OctreeNode* octree_root;
    int max_nodes;
    int node_count;
    int num_samples;
    HemisphericalSampler HS = HemisphericalSampler();
    static RendererCaching* instance;

public:

    RendererCaching(Shader* shader);

    //Builds the Octree where the sample points are stored
	void IrradianceCache(Camera*& cam, Film* film,
		std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList);

    void subdivideAndCache(OctreeNode* node, Camera* cam, Film* film, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList);

    void ComputeIrradiance(OctreeNode* node, Camera* cam, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList);

    void paintAll(Film* film, Camera*& cam, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList); //For visoalization
    void paintLeafNodesWhite(OctreeNode* node, Film* film);

    Vector3D GetIrradiance(const Vector3D& point, const Vector3D& normal);

    bool isPointInsideRegion(const Vector3D& point, const Vector3D& center, double size);

    Vector3D interpolateIrradiance(const Vector3D& p, const Vector3D& n, std::vector<std::shared_ptr<OctreeNode>> nodes, double a = 1.0);

    Vector3D computeTranslationalGradient(const Vector3D& p, const Vector3D& pi);// , const Vector3D& transGrad);
    Vector3D computeRotationalGradient(const Vector3D& n, const Vector3D& ni);// , const Vector3D& rotGrad);

    bool IrradianceGradientHigherThanThreshold(std::vector<std::shared_ptr<OctreeNode>>& nodes, std::vector<Shape*>* objectsList);

    double ComputeAverageDistanceToSurfaces(std::vector<std::shared_ptr<OctreeNode>>& nodes, std::vector<Shape*>* objectsList);

    static RendererCaching* getInstance();

    Shader* shader;

};

#endif //CACHING_H
