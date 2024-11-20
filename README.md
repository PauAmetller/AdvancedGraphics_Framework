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

---

## **Irradiance Caching**  

The **Irradiance Caching** feature has been implemented in the `RendererCaching` module. Key aspects include:  
- The structure used to save cache points is an **_octree_**.  
- For gradient calculation, **_Split Sphere Gradients_** were utilized.  
- **_Interpolation of irradiance_** is done using weights that depend on the orientation and distance.  

---

### **Configuration**  
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

