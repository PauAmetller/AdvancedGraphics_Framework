# **AdvancedGraphics_Framework**  
**_Ray Tracing_**

---

## **Description**  
This project builds upon the initial framework provided in the _Advanced Computer Graphics_ course at **University Pompeu Fabra (UPF)**.  

- **Commit Reference:**  
  The second commit includes files from the framework provided during the course.  

- **Implemented Features:**  
  - Implementation of different shaders to calculate **Global Illumination**, from which the most remarkable are:  
    - _Pure Path Tracing_  
    - _Next Event Estimation_
   
  ### **Configuration for Path Tracing Shader Selection**  
To enable and configure the path tracing shader, take note of the following variables in `main.cpp`:  

- **`has_mirror`:**  
  To activate or deactivate the mirror material. (If deactivated, it will automatically be set as a diffuse material)

- **`has_transmissive`:**  
  To activate or deactivate the transmissive material. (If deactivated, it will automatically be set as a diffuse material)

- **`shader_name`:**  
  Used to select the shader from the following options:
  - **`"intersection"`**: Displays intersection points of rays with objects in the scene.
  - **`"depth"`**: Visualizes the depth (distance) of objects from the camera.
  - **`"normal"`**: Displays the surface normals at the intersection points.
  - **`"whitted"`**: Implements Whitted-style ray tracing, including reflections and refractions.
  - **`"HDI"`**: Uses Hemisphere Direct Illumination for more realistic lighting.
  - **`"ADI"`**: Uses Area Direct Illumination for improved ambient light simulation.
  - **`"PPT"`**: Uses Pure Path Tracing for full global illumination.
  - **`"NEE"`**: Uses Next Event Estimation for better efficiency in sampling light sources.

---

## **Irradiance Caching**  

The **Irradiance Caching** feature has been implemented in the `RendererCaching` module. Key aspects include:  
- The structure used to save cache points is an **_octree_**.  
- For gradient calculation, **_Split Sphere Gradients_** were utilized.  
- **_Interpolation of irradiance_** is done using weights that depend on the orientation and distance.  

### **Configuration for Irradiance Caching**  
To enable and configure irradiance caching, take note of the following variables in `main.cpp`:  

1. **`rendering_mode`:**  
   Set this variable to `"Caching"` to activate the irradiance caching mode.  

2. **Booleans:**  
   - **`only_irradiance`:**  
     - If **`true`**: Only the irradiance will be shown in the rendered image.  
     - If **`false`**: The diffuse and emissive components will also be applied.  
     
   - **`samples_seen`:**  
     - Controls the visibility of cache points in the rendered image:  
       - **`true`**: Pixels corresponding to cache points will appear white.  
       - **`false`**: Cache points will not be visible.  

---

