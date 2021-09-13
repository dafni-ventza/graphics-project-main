# Rain Project simulation

- Fresnel shader added
- Collision detection between particles and terrain for initial approach of marching cubes topic in fluid simulation
- Camera adjusted for optimal view
- Movable model in the scene
- Raindrops made more realistic
- Cloud simulation - adjusted per time and with wind --> fog effect
- Wind simulation (U/I Keys pressed for adjusting in direction X and Z respectively
- Phong added
- Texture mapped models (scene, cloud, rain droplets)

Model created via Unity, Tangram Heightmapper (https://tangrams.github.io/heightmapper/#8.88889/-7.9905/27.5196) and GIMP tools.

# Model creation
  The model was created in Unity Engine in the way described below:
  1. Finding a mountain of our interest in Google Maps
  2. Getting a screengrab of the satellite 3D height view
  3. Adding the coordinates to the Tangram Heightmapper
  4. Downloading everything and putting together into GIMP after image processing work was done (flattening, gray threshold, contrast and tone mapping)
  5. Exporting from Unity the obj created

  Image processing and Unity export types additional resources:
  - https://www.buildandshoot.com/forums/viewtopic.php?t=4984 
  - https://forum.unity.com/threads/export-unity-mesh-to-obj-or-fbx-format.222690/
  - https://www.gamedev.net/forums/topic/669913-open-source-height-map-generator/5239615/
  - https://styly.cc/tips/terrain-obj-exporter/


# First view
At first place the user sees the screen with the fog extended all over the scene view. Moving around with the key arrows we are able to approach the mountain in a scenematic way. Initial number of particles can be increased by pressing M or decressed by pressing L - changing the particle life attribute in our particle system.
Raindrops are falling vertically in our scene and are simulated using a sphere with an added water png as texture. By fixing the transparency of the droplet in particle.fragmentshader more realistic results are achieved. Also, the raindrops are of a random mass. 

![image](https://user-images.githubusercontent.com/42025441/133143765-f80946cc-c7f0-48b0-9dde-b1e6986dfecf.png)

The fog is achieved by using the fountain emitter and rendering having the following settings set up:

  glEnable(GL_DEPTH_TEST); //Depth sense
	glDisable(GL_CULL_FACE); //Cull affecting the rendering based upon the window coordinates
  glEnable(GL_BLEND); //In order to the computed fragment color values with the values in the color buffers.  - FOR TRANSPARENT OBJ
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Transparency of objects via fragment shader fixes
  
Having the clouds - fountain_emitter - change based upon time we are achieving the effect of getting shadows and different lighting on our scene.

![image](https://user-images.githubusercontent.com/42025441/133144609-b7b7fcfb-c7af-4710-9eac-9c411ccac1fe.png)
![image](https://user-images.githubusercontent.com/42025441/133144627-e78dd9e4-c75f-4522-a3bc-9c73954ebc64.png)
![image](https://user-images.githubusercontent.com/42025441/133144640-7840d678-bddc-4942-a03a-19d714bb3ff2.png)


Having also applied wind the result brings the droplets towards the camera or expanding throughout the field.
![image](https://user-images.githubusercontent.com/42025441/133145144-e6e255e5-1b6a-4188-99c0-2bb88b56cfd7.png)


# Without the fog for clearer view
![image](https://user-images.githubusercontent.com/42025441/133142113-e0e913a9-7bd1-4ab7-a008-26046ffddf7c.png)
Adding more droplets in our scene and also moving closer to the mountain-terrain model using WASDEQ keybuttons (E,Q gives the opportunity to move up and down in the y axis) 
![image](https://user-images.githubusercontent.com/42025441/133142375-3b0b2da8-bec1-46e6-b4a2-7448ab2563ee.png)
Using U/I there is a change in the axis droplet fall as shown in the picture.
Code: 
void windXManipulation() {

	particle.factorXWind += Random::Float() *particle.factorXWind;
	
	particle.velocity = glm::vec3(5 - Random::Float() * 5, -particle.speedYDroplet, 5 - Random::Float() * 10)*particle.factorXWind;

}

void windZManipulation() {
	
	particle.factorZWind += Random::Float() * particle.factorZWind;

	particle.velocity = glm::vec3(5 - Random::Float() * 10, -particle.speedYDroplet, 5 - Random::Float() * 5)*particle.factorZWind;

	particle.rot_axis = glm::normalize(glm::vec3(
	0.0f,
	0.0f,
	1 - 2 * Random::Float()));
}

![image](https://user-images.githubusercontent.com/42025441/133143069-0b21b8a7-e75a-4b6b-a99d-e3ee2d44e3e2.png)


# Marching cubes algorithm

In order to be able to simulate any fluid usually the marching cubes algorithm is followed with variations. One variation is the metaballs which could lead to higher performance for example. 
In the lab.cpp source file the MarchingCube algorithm is written, but yet again nowhere called due to exception errors in the current implementation.

<code>
	float MarchingCubes(float x, float y, float z, float r, vec4 c) 
	{
		float value, distanceSquared;
		float resultSquared = r * r;

		//Finding the distance between particle position c and the evaluation point (x,y,z)
		distanceSquared /= resultSquared;

		//Optimization function - metaball approach
		value = distanceSquared * distanceSquared - distanceSquared + 0.25f;

		if (distanceSquared > height_threshold) 
		{
			value = 0.0f;
		}

	return value;
	
	}
	

	

# Random function
In the Random.cpp and Random.h, a better a approach to generating random numbers is achieved via distribution pools affected by the device seeds. 

  #include "common/Random.h"

  std::mt19937 Random::s_RandomEngine;

  std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

------------------------------------------------

  #pragma once

  #include <random>

  class Random {
  
    //using this for generating any random numbers desired
    //Random::Float() * 100.0f; //generatate a random number from 0-100;

    public:
      static void Init() {
        s_RandomEngine.seed(std::random_device()());
      }

    static float Float() {
      return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
    }

    private:
      static std::mt19937 s_RandomEngine;
      static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
    };


# Fresnel Effect Reflection
  
The method is written in the fresnel shaders, although the outcomes are not achieved due to issues in matrices propagated from the vertex shader and being misused in the main program (lab.cpp). Yet again we are trying to reflect both for the rain particles and the cloud. 
  
# Blinn-Phong method lighting
Added in the standard fragment shader.
  


# Camera
  
  Camera::Camera(GLFWwindow* window) : window(window) 	
  {
	
    position = glm::vec3(0, 0, 40);
	
    horizontalAngle = 3.14f;
	
    verticalAngle = 0.0f;
	
    FoV = 67.0f;
	
    speed = 3.0f;
	
    mouseSpeed = 0.001f;
	
    fovSpeed = 2.0f;
	
  }
  
  
  Finding the projection matrix using the matrices defined in theory. What is important to be mentioned is the clipping plane numbers, due to the model designed and implemented in the project. Usually the field of view works in 67 degrees (fixed to radians following the camera methods described by Anton Gerdelan in his book). Nevertheless, the clipping plane is usually set to 0.1f for near and 100.0f for further. In this project, due to the models that is not the case and the process is defined in the code below on how to find the projection matrix.
  
  	float near = 0.001f; // clipping plane
	
	float far = 5400.0f; // clipping plane
	
	float aspect = 4 / 3.0f; // aspect ratio

	// matrix components
	float range = tan(FoV * 0.5f) * near;
	float Sx = (2.0f * near) / (range * aspect + range * aspect);
	float Sy = near / range;
	float Sz = -(far + near) / (far - near);
	float Pz = -(2.0f * far * near) / (far - near);

	projectionMatrix = {
	Sx, 0.0f, 0.0f, 0.0f,
	0.0f, Sy, 0.0f, 0.0f,
	0.0f, 0.0f, Sz, -1.0f,
	0.0f, 0.0f, Pz, 0.0f
	};
  
