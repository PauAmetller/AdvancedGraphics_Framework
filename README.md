# AdvancedGraphics_Framework
 Ray Tracing


**Description**
This project builds upon the initial framework provided in the Advanced Computer Graphics course at University Pompeu Fabra (UPF).
Commit Reference: The second commit includes files from the framework provided during the course.
Implementation of diferent shaders to calculate Global Ilumination inclusing Pure Path Tracing and Next Event Estimation.


**Irradiance Caching**
Implementation of irradiance caching in the RendererCaching, the structure to save the cache point is an octree and for the gradient calculation we used the Split Sphere Gradients, the interpolation of the irradiance is still in process.
We have a **few variables** in **main.cpp** that we must take into account to run the Irradiance caching;
· First, to run the irradiance caching, you should change the variable **rendering_mode** to be equal to **"Caching"**
· Then, we have two booleans;
    · **only_irradiance:** If this is true, only the irradiance will be shown in the rendered image; otherwise, the diffuse and emissive components will also be applied.
    · **samples_seen:** This is used to control whether you want the pixels of the cache points to be printed white on the rendered image, or if you don’t want them to be seen at all.
