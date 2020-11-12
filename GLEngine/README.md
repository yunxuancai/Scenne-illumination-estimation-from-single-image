GLEngine
======
GLEngine is a C++ OpenGL graphics engine which aimed to be a skill, knowledge and possibly graphical showcase. It will support many graphical features, from something as basic as textures and shaders to deferred rendering, PBR and such.

Link to the demo reel : https://vimeo.com/200574427

Screenshots
------

* Example using Image-Based Lighting :

![](https://image.ibb.co/nt76La/Screenshot_from_2017_02_23_13_26_44.png)
* G-Buffer structure :

![](http://image.noelshack.com/fichiers/2016/52/1483180466-glengine2.png)


Features
------

* Camera :
    * Movements
    * Zoom in/out
    * Exposure :
        * Aperture
        * Shutter speed
        * ISO
    * **TODO :** Other physically based camera parameters

* Textures :
    * Init/loading/binding from anywhere
	* Anisotropic Filtering
    * HDR
    * Cubemap

* Material :
	* PBR material pipeline :
		* Albedo
        * Normal
		* Roughness
		* Metalness
		* AO
	* **TODO :** Informations seamlessly sent to the G-Buffer

* Models/Meshes :
    * External models loading (.obj)
    * **TODO :** PBR material pipeline compliant
    * **TODO :** Revamp & debug to support kinda buggy models

* Basic Shapes :
    * Simple shape generation (quad, plane, cube)
    * Predefined array based
    * **TODO :** Shape topology computation (sphere...)

* Shaders :
    * Init/loading/binding from anywhere
    * G-Buffer support
    * PBR material pipeline compliant
	* **TODO :** UBOs

* Skybox :
    * 6-faced cubemap based
    * Screenquad based
    * Equirectangular/spherical maps
    * HDR
    * Deferred Rendering compliant
    
* Resources Manager :
    * **TODO :** Resources (textures, shaders, models, materials...) centralization

* Lights :
    * Point light
    * Directional light
    * **TODO :** Spot light

* Lighting :
    * Cook-Torrance BRDF
    * Deferred Rendering
    * **TODO :** Shadow-mapping (PCF/Variance)
    * **TODO :** Tiled Deferred Rendering (Compute shaders ?)

* PBR Pipeline :
    * BRDF :
        * Cook-Torrance model
        * Diffuse : Lambertian/Disney
        * Fresnel term : Schlick
        * Microfacet distribution : GGX
        * Geometry attenuation : GGX-Smith
    * Material pipeline using a roughness/metalness workflow
    * Image-Based Lighting (Epic split-sum method) :
        * Diffuse irradiance
        * Specular radiance

* Post-processing :
    * Scalable Ambient Obscurance (SAO)
    * FXAA
    * Motion Blur (camera/per-fragment)
    * Tonemapping (Reinhard, Filmic, Uncharted)
	* **TODO :** Bloom
	* **TODO :** Depth of Field
	* **TODO :** Screen-Space Reflections
	* **TODO :** Lens Flare
	* **TODO :** Eye Adaptation
 
* Utility :
    * GUI using ImGui
    * Basic/naive GPU profiling
    * G-Buffer visualization for debugging purpose
	* Borderless Fullscreen
    * **TODO :** Logging
    * **TODO :** CPU profiling
    * **TODO :** G-Buffer export as .png
    * **TODO :** GUI using Qt5 (which imply a whole project revamping)

How to use
------
GLEngine was written using Linux, QtCreator as the IDE, CMake 3.0+ as the building tool, OpenGL 4.0+ as the Graphics API and a C++11 compiler in mind.

Download the source, open the CMakeList.txt file with QtCreator, build the project, and everything should be ready to use.

* In GLEngine :
    * Hold the right mouse button to use the camera and its features
    * Toggle between the different buffers using the 1-9 buttons

Dependencies (included)
------
- Window & Input system : GLFW
- OpenGL Function Loader : GLAD
- OpenGL Mathematic Functions : GLM
- Image Loading : stb
- Mesh Loading : Assimp

Credits
------
- Joey de Vries (LearnOpenGL)
- Kevin Fung (Glitter)
