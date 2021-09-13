#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "model.h"
#include <glm/gtx/string_cast.hpp>
#include <GL/GLU.h>



#define USE_PARALLEL_TRANSFORM

//Gives a random number between 0 and 1
#define RAND ((float) rand()) / (float) RAND_MAX

//particleProp = particleAttributes

struct particleAttributes {
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rot_axis = glm::vec3(0, 0, 1);
	float rot_angle = 0.0f; //degrees
	glm::vec3 accel = glm::vec3(0, 0, 0); //velocity variation
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	float life = 1.0f; //lifetime
	float mass = 0.0f; //sizevariation
	float SizeBegin, SizeEnd;
	glm::vec4 ColorBegin, ColorEnd;

	//Controlling the wind
	float speedYDroplet = 0.2f;
	float factorXWind = 1.1f;
	float factorZWind = 0.1f;
	float speedDropFall = 0.0003f;



	float dist_from_camera = 0.0f; //In case you want to do depth sorting
	bool operator < (const particleAttributes & p) const
	{
		return dist_from_camera > p.dist_from_camera;
	}
};

//Particle Emitter is our ParticleSystem!

//ParticleEmitterInt is an interface class. Emitter classes must derive from this one and implement the updateParticles method
class IntParticleEmitter
{
public:
	GLuint emitterVAO;
	int number_of_particles;

	std::vector<particleAttributes> p_attributes;

	bool use_rotations = true;
	bool use_sorting = true;


	glm::vec3 emitter_pos; //the origin of the emitter

	//IntParticleEmitter();
	


	IntParticleEmitter(Drawable* _model, int number);
	void changeParticleNumber(int new_number);

	void renderParticles(int time = 0);
	virtual void updateParticles(float time, float dt, glm::vec3 camera_pos) = 0;
	virtual void createNewParticle(int index) = 0;

	glm::vec4 calculateBillboardRotationMatrix(glm::vec3 particle_pos, glm::vec3 camera_pos);

	/*IntParticleEmitter();

	void OnUpdate(GLCore::Timestep ts);
	void OnRender(GLCore::Utils::OrthographicsCamera& camera);

	void Emit(const particleAttributes &particleAttributes);*/

private:

	std::vector<glm::mat4> translations;
	std::vector<glm::mat4> rotations;
	std::vector<float> scales;
	std::vector<float> lifes;

	// A particle at any given frame...
	struct Particle {
		glm::vec3 Position;
		glm::vec3 Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd;

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;
	};
	
	//faster creation of particles
	//everything is continuous in memory, running faster
	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex = 999;

	GLuint m_QuadVA = 0;
	//

	Drawable* model;
	void configureVAO();
	void bindAndUpdateBuffers();
	GLuint transformations_buffer;
	GLuint rotations_buffer;
	GLuint scales_buffer;
	GLuint lifes_buffer;

};

