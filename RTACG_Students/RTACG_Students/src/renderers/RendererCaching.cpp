#include "RendererCaching.h"

#define M_PI 3.14159265358979323846

RendererCaching* RendererCaching::instance = nullptr;

//Constructor
RendererCaching::RendererCaching(Shader* shader) {
    this->max_nodes = 100000;
    this->node_count = 0;
    this->shader = shader;
    this->num_samples = 254;

    if (instance == nullptr) {
        instance = this; // Set the current object as the singleton instance
    }
};

//Builds the Octree where the sample points are stored
void RendererCaching::IrradianceCache(Camera*& cam, Film* film,
    std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList, bool only_irradiance, bool samples_seen) {

    size_t X = (film->getWidth() + 0.5) / 2.0;
    size_t Y = (film->getHeight() + 0.5) / 2.0;

    // Compute the pixel position in NDC
    double x = (double)(X) / film->getWidth();
    double y = (double)(Y) / film->getHeight();

    Ray cameraRay = cam->generateRay(X, Y);
    Intersection its;
    if (Utils::getClosestIntersection(cameraRay, *objectsList, its))
    {
        this->octree_root = new OctreeNode(its.itsPoint, max(X, Y), its, X, Y);
        node_count++;
        //if(octree_root == nullptr)
        std::cerr << "Point (" << octree_root->col << ", " << octree_root->lin 
            << ") Size;" <<octree_root->size<< "\n";
       
        subdivideAndCache(this->octree_root, cam, film, objectsList, lightSourceList);
    }
    else {
        // Discard point if it doesn't intersect any surface
        std::cerr << "Point (" << its.itsPoint.x << ", " << its.itsPoint.y << ", " << its.itsPoint.z
            << ") does not intersect any surface.\n";
    }
    std::cerr << this->node_count << std::endl;

    // Once we've finished subdividing and caching, we need to paint the leaf nodes white
    paintAll(film, cam, objectsList, lightSourceList, only_irradiance);
    if (samples_seen) {
        paintLeafNodesWhite(this->octree_root, film);
    }
}

void RendererCaching::subdivideAndCache(OctreeNode* node, Camera* cam, Film* film, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList) {

    ComputeIrradiance(node, cam, objectsList, lightSourceList);

    node->subdivide(cam, objectsList, lightSourceList, film->getWidth(), film->getHeight());

    // Show progression 
    double progress = (double)this->node_count / double(this->max_nodes);
    Utils::printProgress(progress);

    if (this->node_count < this->max_nodes) {
        for (const std::shared_ptr<OctreeNode>& children_node : node->children) {
            if (children_node.get() != nullptr) {
                subdivideAndCache(children_node.get(), cam, film, objectsList, lightSourceList);
            }
            else {
                std::cerr << "Warning: Null child node encountered in subdivision!" << std::endl;
            }
        }
    }

}

void RendererCaching::ComputeIrradiance(OctreeNode* node, Camera* cam, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList) {

    Vector3D radiance;
    double weightSum = 0.0;

    for (int i = 0; i < this->num_samples; i++) {
        Vector3D sample_direction = HS.getSample(node->its.normal);
        Ray sample_ray = Ray(node->its.itsPoint, sample_direction, 1);
        Intersection its;

        if (Utils::getClosestIntersection(sample_ray, *objectsList, its))
        {
            radiance = this->shader->ComputeRadiance(sample_ray, *objectsList, *lightSourceList, sample_ray.depth);
            node->irradiance += radiance * dot(node->its.normal, sample_direction);

            weightSum += 1.0;
        }
    }

    if (weightSum > 0.0) {
        node->irradiance /= weightSum;
    }
}


void RendererCaching::paintAll(Film* film, Camera*& cam, std::vector<Shape*>*& objectsList, std::vector<LightSource*>*& lightSourceList, bool only_irradiance) {

    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    Shader* color_shader = new WhittedIntegrator(shader->bgColor);

    for (size_t lin = 0; lin < resY; lin++)
    {
        // Show progression 
        double progress = (double)lin / double(resY);
        Utils::printProgress(progress);

        // Inner loop invariant: we have rendered col columns
        for (size_t col = 0; col < resX; col++)
        {
            // Compute the pixel position in NDC
            double x = (double)(col + 0.5) / resX;
            double y = (double)(lin + 0.5) / resY;
            // Generate the camera ray
            Ray cameraRay = cam->generateRay(x, y);
            Vector3D pixelColor = Vector3D(0.0);

            //printf("New Ray \n");
            Intersection its;
            if (Utils::getClosestIntersection(cameraRay, *objectsList, its)) {
                if (only_irradiance) {
                    pixelColor += GetIrradiance(col, lin, its.itsPoint, its.normal, objectsList);
                }
                else {
                    const Material& material = its.shape->getMaterial();
                    pixelColor += GetIrradiance(col, lin, its.itsPoint, its.normal, objectsList) * material.getDiffuseReflectance();
                    pixelColor += material.getEmissiveRadiance();
                }
            }

            // Store the pixel color
            film->setPixelValue(col, lin, pixelColor);
        }
    }
}

void RendererCaching::paintLeafNodesWhite(OctreeNode* node, Film* film) {
    if (node->isLeaf) {

        // Ensure the pixel is within the bounds of the film (image)
        if (node->col >= 0 && node->col < film->getWidth() && node->lin >= 0 && node->lin < film->getHeight()) {
            //std::cerr <<  node->col <<  "hola " <<  node->lin << std::endl;
            film->setPixelValue(node->col, node->lin, Vector3D(1.0));
        }
    }

    // Recurse to children if they exist
    for (const std::shared_ptr<OctreeNode>& child_node : node->children) {
        paintLeafNodesWhite(child_node.get(), film);
    }
}

// Function to find irradiance at a point by traversing the octree
Vector3D RendererCaching::GetIrradiance(double col, double lin, const Vector3D& point, const Vector3D& normal, std::vector<Shape*>*& objectsList) {

    // Radius of influence for nearby nodes
    const double radiusOfInfluenceFactor = 5;  // Adjust based on application needs

    std::vector<OctreeNode*> nodesToCheck = { this->octree_root };
    std::vector<OctreeNode*> leafNodes;
    OctreeNode* currentNode;

    double max_size = this->octree_root->size;


    //////This part gets a memory leak, (poccibly a destroy, when trying to pop an inexistent node)////
    while (!nodesToCheck.empty()) {
        currentNode = nodesToCheck.back();
        nodesToCheck.pop_back();

        // Compute distance from the point to the current node's center
        double distanceToNode = (point - currentNode->center).length();

        double effectiveRadiusOfInfluence = radiusOfInfluenceFactor * (currentNode->size / max_size);

        // Check if the point is within the node's region or nearby
        if (isPointInsideRegion(col, lin, currentNode->col, currentNode->lin, currentNode->size) || distanceToNode <= effectiveRadiusOfInfluence) {

            // If it's a leaf node, consider it for interpolation
            if (currentNode->isLeaf) {
                leafNodes.push_back(currentNode);
            }
            else {
                // Add children to the stack for further traversal
                for (const std::shared_ptr<OctreeNode>& child_ : currentNode->children) {
                    OctreeNode* child = child_.get();
                    if (child) { // Ensure child is not null
                        nodesToCheck.push_back(child);
                    }
                }
            }
        }
    }

    // Interpolate irradiance from the candidate nodes
    if (!leafNodes.empty()) {
        return interpolateIrradiance(point, normal, leafNodes, 0.3, 5.0, 0.001);
    }
    else {
        return Vector3D(0.0);  // Return 0 if no suitable nodes are found
    }
}

bool RendererCaching::isPointInsideRegion(double col, double lin, double node_col, double node_lin , double size) {
    double halfSize = size * 20;
    return (col >= node_col - halfSize && col <= node_col + halfSize &&
        lin >= node_lin - halfSize && lin <= node_lin + halfSize);
}


Vector3D RendererCaching::interpolateIrradiance(const Vector3D& p, const Vector3D& n, std::vector<OctreeNode*> nodes, double alpha, double beta, double minWeight) {
    Vector3D numerator = { 0.0, 0.0, 0.0 };
    double denominator = 0.0;

    for (const auto& node : nodes) {
        Vector3D pi = node->center;
        Vector3D ni = node->its.normal;
        Vector3D Ei = node->irradiance;

        // Compute distance and orientation differences
        double distance = (p - pi).length();
        double orientation = 1.0 - dot(n.normalized(), ni.normalized());

        // Weight using Gaussian falloff for both distance and orientation
        double weight = exp(-alpha * distance * distance) * exp(-beta * orientation * orientation);

        // Apply tolerance filter
        if (weight >= minWeight) {
            // Accumulate irradiance contribution
            numerator = numerator + (Ei * weight);
            denominator += weight;
        }
    }

    // Prevent division by zero with a small epsilon threshold
    const double EPSILON = 1e-6;
    if (denominator < EPSILON) return Vector3D{ 0.0, 0.0, 0.0 };

    // Return interpolated irradiance
    return numerator / denominator;
}


//////////////////////////////////Not used////////////////////////// I will use it if I can complete them correctly and relate them to the gradient and interpolation.
// Compute rotational gradient contribution
Vector3D RendererCaching::computeRotationalGradient(const Vector3D& n, const Vector3D& ni) { //, const Vector3D& rotGrad) {
    return cross(n, ni);// *rotGrad.magnitude();
}

// Compute translational gradient contribution
Vector3D RendererCaching::computeTranslationalGradient(const Vector3D& p, const Vector3D& pi) { //, const Vector3D& transGrad) {
    return (p - pi);// *transGrad.magnitude();
}
///////////////////////////////////////////////////////////////////

bool RendererCaching::IrradianceGradientHigherThanThreshold(std::vector<std::shared_ptr<OctreeNode>>& nodes, std::vector<Shape*>* objectsList) { // Change it its not working correctly

        const double threshold = 0.01;  // Gradient threshold
        const double R0 = ComputeAverageDistanceToSurfacesShared(nodes, objectsList);  // Precompute R0

        for (size_t i = 0; i < nodes.size(); ++i) {
            auto node1 = nodes[i];
            if (!node1) continue;

            for (size_t j = i + 1; j < nodes.size(); ++j) {
                auto node2 = nodes[j];
                if (!node2) continue;

                // Compute irradiance difference
                Vector3D irradiance_diff = node1->irradiance - node2->irradiance;

                // Spatial difference
                double spatial_diff = (node1->center - node2->center).length() / R0;

                // Angular difference
                Vector3D normal1 = node1->its.normal;
                Vector3D normal2 = node2->its.normal;
                double angular_diff = std::sqrt(2.0 - 2.0 * dot(normal2, normal1));

                // Total gradient
                double gradient = irradiance_diff.length() * ((4.0 / M_PI) * spatial_diff + angular_diff);

                /////////////////////////////////////////////////
                //Implement if possible at the end if time
                ////////////////////////////////////////////
                /*
                // Incorporate rotation and translation gradients (weighted appropriately)
                Vector3D rotGradient1 = computeRotationalGradient(normal1, normal2);// , rotGrad1);
                Vector3D rotGradient2 = computeRotationalGradient(normal2, normal1);// , rotGrad2);


                Vector3D transGradient1 = computeTranslationalGradient(node1->center, node2->center);// , transGrad1);
                Vector3D transGradient2 = computeTranslationalGradient(node1->center, node2->center);// , transGrad2);

                // Add rotation and translation gradient contributions (weighted)
                gradient += rotGradient1.length() + rotGradient2.length();
                gradient += transGradient1.length() + transGradient2.length();

                // Normalize spatial and angular differences if necessary
                gradient /= (1.0 + spatial_diff + angular_diff);  // Avoid runaway gradient
                */
                ///////////////////////////////////////////////////////////////////////////

                // Compare with threshold
                if (gradient > threshold) {
                    return true;
                }
            }
        }
        return false;
}

double RendererCaching::ComputeAverageDistanceToSurfacesShared(std::vector<std::shared_ptr<OctreeNode>>& nodes, std::vector<Shape*>* objectsList) {
    const int numSamplesPerNode = 64; // Number of rays per node
    double totalDistance = 0.0;
    int totalHitCount = 0;

    for (const auto& node : nodes) {
        const Vector3D center = node->center;
        double nodeTotalDistance = 0.0;
        int nodeHitCount = 0;

        for (int i = 0; i < numSamplesPerNode; ++i) {
            // Generate a random direction on the unit sphere
            Vector3D direction = HS.getSample(node->its.normal);

            // Cast a ray from the node's center in the sampled direction
            Ray ray(center, direction);
            Intersection its;

            // Check for intersection with objects in the scene
            if (Utils::getClosestIntersection(ray, *objectsList, its)) {
                // Accumulate distance to the intersection point
                double distance = (its.itsPoint - center).length();
                nodeTotalDistance += distance;
                nodeHitCount++;
            }
        }

        // If this node has any hits, add its contribution to the global average
        if (nodeHitCount > 0) {
            totalDistance += nodeTotalDistance;
            totalHitCount += nodeHitCount;
        }
    }


    // Compute global average distance, avoid division by zero
    if (totalHitCount > 0) {
        return totalDistance / totalHitCount ;
    }
    else {
        // Default value if no surfaces are hit
        return 1e10; // A very large value to signify no nearby surfaces
    }
}

RendererCaching* RendererCaching::getInstance(){
    return instance;
}


