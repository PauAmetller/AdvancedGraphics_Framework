# AdvancedGraphics_Framework
 Ray Tracing


**Description**
This project builds upon the initial framework provided in the Advanced Computer Graphics course at University Pompeu Fabra (UPF).
Commit Reference: The second commit includes files from the framework provided during the course.
Implementation of diferent shaders to calculate Global Ilumination inclusing Pure Path Tracing and Next Event Estimation.


**Irradiance Caching**
Implementation of irradiance caching in the RendererCaching, the structure to save the cache point is an octree and for the gradient calculation we used the Split Sphere Gradients, the interpolation of the irradiance is still in process.
To run the irradiance caching you should change the variable **rendering_mode** to be equal to **"Caching"**
