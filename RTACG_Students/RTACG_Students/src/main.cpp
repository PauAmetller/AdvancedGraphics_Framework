#include <iostream>
#include <algorithm>

#include "core/film.h"
#include "core/matrix4x4.h"
#include "core/ray.h"
#include "core/utils.h"
#include "core/scene.h"


#include "shapes/sphere.h"
#include "shapes/infiniteplan.h"

#include "cameras/ortographic.h"
#include "cameras/perspective.h"

#include "shaders/intersectionshader.h"
#include "shaders/depthshader.h"
#include "shaders/normalshader.h"
#include "shaders/whittedshader.h"
#include "shaders/hemisphericaldirectilluminationshader.h"
#include "shaders/areadirectilluminationshader.h"
#include "shaders/purepathtracing.h"
#include "shaders/nexteventestimation.h"

#include "renderers/RendererCaching.h"

#include "materials/phong.h"
#include "materials/emissive.h"
#include "materials/mirror.h"
#include "materials/transmissive.h"

#include <chrono>

using namespace std::chrono;

typedef std::chrono::duration<double, std::milli> durationMs;

bool has_mirror = false;
bool has_transmissive = false;
char* shader_name = "NEE";   //intersaction, depth, normal, whitted, HDI, ADI, PPT, NEE
char* rendering_mode = "Caching"; //Caching, not_caching_NEE_for_comparation, Whitout Caching
char* shader_DI_for_caching_renderer = "NEE";  //ADI, NEE, None(If none the diffuse material is applyed)
bool only_irradiance = true;
bool samples_seen = false;


void buildSceneCornellBox(Camera*& cam, Film*& film,
    Scene myScene)
{
    /* **************************** */
/* Declare and place the camera */
/* **************************** */
    Matrix4x4 cameraToWorld = Matrix4x4::translate(Vector3D(0, 0, -3));
    double fovDegrees = 60;
    double fovRadians = Utils::degreesToRadians(fovDegrees);
    cam = new PerspectiveCamera(cameraToWorld, fovRadians, *film);

    /* ********* */
    /* Materials */
    /* ********* */
    Material* redDiffuse = new Phong(Vector3D(0.7, 0.2, 0.3), Vector3D(0, 0, 0), 100);
    Material* greenDiffuse = new Phong(Vector3D(0.2, 0.7, 0.3), Vector3D(0, 0, 0), 100);
    Material* greyDiffuse = new Phong(Vector3D(0.8, 0.8, 0.8), Vector3D(0, 0, 0), 100);      
    Material* blueGlossy_20 = new Phong(Vector3D(0.2, 0.3, 0.8), Vector3D(0.2, 0.2, 0.2), 20);
    Material* blueGlossy_80 = new Phong(Vector3D(0.2, 0.3, 0.8), Vector3D(0.2, 0.2, 0.2), 80);
    Material* cyandiffuse = new Phong(Vector3D(0.2, 0.8, 0.8), Vector3D(0, 0, 0), 100);
    Material* emissive = new Emissive(Vector3D(25, 25, 25), Vector3D(0.5));

    //Task 5.3
    Material* mirror = new Mirror();
    //Task 5.4
    Material* transmissive = new Transmissive(0.7);


    /* ******* */
    /* Objects */
    /* ******* */
    double offset = 3.0;
    Matrix4x4 idTransform;
    // Construct the Cornell Box
    Shape* leftPlan = new InfinitePlan(Vector3D(-offset - 1, 0, 0), Vector3D(1, 0, 0), redDiffuse);
    Shape* rightPlan = new InfinitePlan(Vector3D(offset + 1, 0, 0), Vector3D(-1, 0, 0), greenDiffuse);
    Shape* topPlan = new InfinitePlan(Vector3D(0, offset, 0), Vector3D(0, -1, 0), greyDiffuse);
    Shape* bottomPlan = new InfinitePlan(Vector3D(0, -offset, 0), Vector3D(0, 1, 0), greyDiffuse);
    Shape* backPlan = new InfinitePlan(Vector3D(0, 0, 3 * offset), Vector3D(0, 0, -1), greyDiffuse);


    myScene.AddObject(leftPlan);
    myScene.AddObject(rightPlan);
    myScene.AddObject(topPlan);
    myScene.AddObject(bottomPlan);
    myScene.AddObject(backPlan);

    Shape* square_emissive = nullptr;
    if (shader_name == "HDI" || shader_name == "ADI" || shader_name == "PPT" || shader_name == "NEE") {
        square_emissive = new Square(Vector3D(-1.0, 3.0, 3.0), Vector3D(2.0, 0.0, 0.0), Vector3D(0.0, 0.0, 2.0), Vector3D(0.0, -1.0, 0.0), emissive);
        myScene.AddObject(square_emissive);
    }


    // Place the Spheres and square inside the Cornell Box
    double radius = 1;         
    Matrix4x4 sphereTransform1;
    sphereTransform1 = Matrix4x4::translate(Vector3D(1.5, -offset + radius, 6));
    Shape* s1 = new Sphere(radius, sphereTransform1, blueGlossy_20); 

    Matrix4x4 sphereTransform2;
    sphereTransform2 = Matrix4x4::translate(Vector3D(-1.5, -offset + 3*radius, 4));
    Shape* s2 = nullptr;
    if(has_transmissive)
        s2 = new Sphere(radius, sphereTransform2, transmissive);
    else
        s2 = new Sphere(radius, sphereTransform2, blueGlossy_80);

    Shape* square = nullptr;
    if(has_mirror)
        square = new Square(Vector3D(offset + 0.999, -offset-0.2, 3.0), Vector3D(0.0, 4.0, 0.0), Vector3D(0.0, 0.0, 2.0), Vector3D(-1.0, 0.0, 0.0), mirror);
    else
        square = new Square(Vector3D(offset + 0.999, -offset - 0.2, 3.0), Vector3D(0.0, 4.0, 0.0), Vector3D(0.0, 0.0, 2.0), Vector3D(-1.0, 0.0, 0.0), cyandiffuse);

    myScene.AddObject(s1);
    myScene.AddObject(s2);
    myScene.AddObject(square);

    if (shader_name == "whitted") {
        PointLightSource* myPointLight = new PointLightSource(Vector3D(0, 2.5, 3.0), Vector3D(2.0));
        myScene.AddPointLight(myPointLight);
    }
}


void buildSceneSphere(Camera*& cam, Film*& film,
    Scene myScene)
{
    /* **************************** */
      /* Declare and place the camera */
      /* **************************** */
      // By default, this gives an ID transform
      //  which means that the camera is located at (0, 0, 0)
      //  and looking at the "+z" direction
    Matrix4x4 cameraToWorld;
    double fovDegrees = 60;
    double fovRadians = Utils::degreesToRadians(fovDegrees);
    cam = new PerspectiveCamera(cameraToWorld, fovRadians, *film);



    /* ************************** */
    /* DEFINE YOUR MATERIALS HERE */
    /* ************************** */

    Material* green_100 = new Phong(Vector3D(0.2, 0.7, 0.3), Vector3D(0.2, 0.6, 0.2), 50);

    // Define and place a sphere
    Matrix4x4 sphereTransform1;
    sphereTransform1 = sphereTransform1.translate(Vector3D(-1.25, 0.5, 4.0));
    Shape* s1 = new Sphere(1.0, sphereTransform1, green_100);

    // Define and place a sphere
    Matrix4x4 sphereTransform2;
    sphereTransform2 = sphereTransform2.translate(Vector3D(1.25, 0.0, 6));
    Shape* s2 = new Sphere(1.25, sphereTransform2, green_100);

    // Define and place a sphere
    Matrix4x4 sphereTransform3;
    sphereTransform3 = sphereTransform3.translate(Vector3D(1.0, -0.75, 3.5));
    Shape* s3 = new Sphere(0.25, sphereTransform3, green_100);

    // Store the objects in the object list
    myScene.AddObject(s1);
    myScene.AddObject(s2);
    myScene.AddObject(s3);
   
}

void raytrace(Camera* &cam, Shader* &shader, Film* &film,
              std::vector<Shape*>* &objectsList, std::vector<LightSource*>* &lightSourceList)
{
    
    /*double my_PI = 0.0;
    double n_estimations = 0.0;
    unsigned int sizeBar = 40;*/

    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    // Main raytracing loop
    // Out-most loop invariant: we have rendered lin lines
    for(size_t lin=0; lin<resY; lin++)
    {
        // Show progression 
        double progress = (double)lin / double(resY);
        Utils::printProgress(progress);

        // Inner loop invariant: we have rendered col columns
        for(size_t col=0; col<resX; col++)
        {
            // Compute the pixel position in NDC
            double x = (double)(col + 0.5) / resX;
            double y = (double)(lin + 0.5) / resY;
            // Generate the camera ray
            Ray cameraRay = cam->generateRay(x, y);
            Vector3D pixelColor = Vector3D(0.0);

            //printf("New Ray \n");
            // Compute ray color according to the used shader
            pixelColor += shader->computeColor(cameraRay, *objectsList, *lightSourceList);

            // Store the pixel color
            film->setPixelValue(col, lin, pixelColor);
        }
    }
}


//------------TASK 1---------------------//
void PaintImage(Film* film)
{
    unsigned int sizeBar = 40;

    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    // Main Image Loop
    for (size_t lin = 0; lin < resY; lin++)
    {
        // Show progression
        if (lin % (resY / sizeBar) == 0)
            std::cout << ".";

        for (size_t col = 0; col < resX; col++)
        { 
            //CHANGE...()
            Vector3D gradient_color = Vector3D( float(col) /float(resX), float(lin) / float(resY), 0.0);
            film->setPixelValue(col,lin, gradient_color);
           
        }
    }
}

int main()
{
    std::string separator     = "\n----------------------------------------------\n";
    std::string separatorStar = "\n**********************************************\n";
    std::cout << separator << "RT-ACG - Ray Tracer for \"Advanced Computer Graphics\"" << separator << std::endl;

    // Create an empty film
    Film *film;

    if (rendering_mode == "Caching" || rendering_mode == "not_caching_NEE_for_comparation") {
        film = new Film(1024, 1024);   //We make it square to prevent possible problems with the Octree
    }
    else {
        film = new Film(720, 512); //default
    }


    //For IrradianceCaching desactivate mirrors and transmissive materials
    if (rendering_mode == "Caching" || rendering_mode == "not_caching_NEE_for_comparation") {
        has_mirror = false;
        has_transmissive = false;
        shader_name == "NEE";
    }


    // Declare the shader
    Vector3D bgColor(0.0, 0.0, 0.0); // Background color (for rays which do not intersect anything)
    Vector3D intersectionColor(1,0,0);
    Vector3D intersectionColorG(0, 1, 0);
    
    //First Assignment
    Shader* shader = nullptr;
    if (shader) {
        delete shader;  // Deallocate the previously allocated shader
    }

    if (shader_name == "intersaction") {
        shader = new IntersectionShader(intersectionColor, bgColor);
    }
    else if (shader_name == "depth") {
        shader = new DepthShader(intersectionColorG, 7.5f, bgColor);
    }
    else if (shader_name == "normal") {
        shader = new NormalShader(bgColor); 
    }
    else if (shader_name == "whitted") {
        shader = new WhittedIntegrator(bgColor);
    }
    else if (shader_name == "HDI") {
        shader = new HDIShader(bgColor);
    }
    else if (shader_name == "ADI") {
        shader = new ADIShader(bgColor);
    }
    else if (shader_name == "PPT") {
        shader = new PPTShader(bgColor);
    }
    else if (shader_name == "NEE" || rendering_mode == "not_caching_NEE_for_comparation") {
        if (rendering_mode == "Caching") {
            shader = new NEEShader(bgColor, 3);
        }
        else {
            shader = new NEEShader(bgColor);
        }
    }
  

    // Build the scene---------------------------------------------------------
    // 
    // Declare pointers to all the variables which describe the scene
    Camera* cam;
    Scene myScene;
    //Create Scene Geometry and Illumiantion
    if (shader_name == "intersaction" || shader_name == "depth" || shader_name == "normal") {
        buildSceneSphere(cam, film, myScene); //Task 2,3,4;
    }
    else {
        buildSceneCornellBox(cam, film, myScene); //Task 5
    }

    //---------------------------------------------------------------------------

    //Paint Image ONLY TASK 1
    //PaintImage(film);

    //Preferably a shader that calculates the radiance, if not it won't be correct to use it for irradiance caching //use NEE (with out mirrors and transmissives)
    RendererCaching* caching = new RendererCaching(shader, shader_DI_for_caching_renderer);

    // Launch some rays! TASK 2,3,...   
    auto start = high_resolution_clock::now();
    if (rendering_mode == "Caching") {
        caching->IrradianceCache(cam, film, myScene.objectsList, myScene.LightSourceList, only_irradiance, samples_seen);
    }
    else {
        raytrace(cam, shader, film, myScene.objectsList, myScene.LightSourceList);
    }
    auto stop = high_resolution_clock::now();

    

    // Save the final result to file
    std::cout << "\n\nSaving the result to file output.bmp\n" << std::endl;
    film->save();
    film->saveEXR();

    float durationS = (durationMs(stop - start) / 1000.0).count() ;
    std::cout <<  "FINAL_TIME(s): " << durationS << std::endl;

    delete[] shader;

    std::cout << "\n\n" << std::endl;
    return 0;
}
