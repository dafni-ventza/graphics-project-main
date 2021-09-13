#include "FountainEmitter.h"

#include "common/Random.h"
#include <iostream>
#include <algorithm>

#define gg 9.80665f

FountainEmitter::FountainEmitter(Drawable *_model, int number) : IntParticleEmitter(_model, number) {}
float speedYDroplet = 2.0f;

float speedDropFall = 0.0003f;

void FountainEmitter::updateParticles(float time, float dt, glm::vec3 camera_pos) {

	//This is for the fountain to slowly increase the number of its particles to the max amount
	//instead of shooting all the particles at once
	if (active_particles < number_of_particles) {
		int batch = 5;
		int limit = std::min(number_of_particles - active_particles, batch);
		for (int i = 0; i < limit; i++) {
			createNewParticle(active_particles);
			active_particles++;
		}
	}
	else {
		active_particles = number_of_particles; //In case we resized our ermitter to a smaller particle number
	}

	for (int i = 0; i < active_particles; i++) {
		particleAttributes & particle = p_attributes[i];

		if (particle.position.y < (emitter_pos.y - 500.0) || particle.life == 0.0f || checkForCollision(particle)) {
			createNewParticle(i);
		}
		// TO CHECK
	/*	if (particle.position.x < (emitter_pos.x - 800.0f) || particle.life == 0.0f || checkForCollision(particle)) {
			createNewParticle(i);
		}*/
	/*	if (particle.position.z < emitter_pos.z || particle.life == 0.0f || checkForCollision(particle)) {
			createNewParticle(i);
		}*/

		/*if (particle.position.y > height_threshold)
			createNewParticle(i);*/

		//createNewParticle(i);
		//particle.accel = glm::vec3(-particle.position.x, 0.0f, -particle.position.z); //gravity force
		//particle.accel = glm::vec3(0, -gg, 0);
		//																			  //particle.rot_angle += 90*dt; 

		//particle.position = particle.position + particle.velocity*dt + particle.accel*(dt*dt)*0.5f;
		//particle.velocity =  particle.accel*dt + particle.velocity;

		particle.position = emitter_pos + glm::vec3(0, 0.2f, 0);

	
	
	/*	particle.velocity = glm::vec3(
			0,
			-particle.speedDropFall*Random::Float() * 0.002,
			0);*/
		particle.velocity = glm::vec3(gg, -gg, 0);

		/*particle.velocity = glm::vec3(
			5 - Random::Float() * particle.factorXWind,
			-particle.speedYDroplet,
			5 - Random::Float() *particle.factorZWind)*particle.speedDropFall;*/

		//*
		auto bill_rot = calculateBillboardRotationMatrix(particle.position, camera_pos);
		particle.rot_axis = glm::vec3(bill_rot.x, bill_rot.y, bill_rot.z);
		particle.rot_angle = glm::degrees(bill_rot.w);
		//*/
		particle.dist_from_camera = length(particle.position - camera_pos);
		particle.life = (height_threshold - particle.position.y) / (height_threshold - emitter_pos.y);
	}
}

bool FountainEmitter::checkForCollision(particleAttributes& particle)
{
	return particle.position.y < 0.0f;
}

void FountainEmitter::createNewParticle(int index) {

	particleAttributes & particle = p_attributes[index];

	
	//Fix the particle position - spawn throughout the whole FoV
	//particle.position = emitter_pos - glm::vec3(Random::Float() * 5, 0, 0);
	//particle.position = emitter_pos - glm::vec3(Random::Float(), 0, Random::Float());
	//Y -> From how high does the rain fall
	//X -> How wide is the rain-cloud range
	particle.position = emitter_pos + glm::vec3(Random::Float(), Random::Float(), 0);

	
	/*particle.velocity = glm::vec3(
		5 - Random::Float() * particle.factorXWind,
		-particle.speedYDroplet, 
		5 - Random::Float() *particle.factorZWind)*particle.speedDropFall;*/
	
	particle.velocity = glm::vec3(
		0,
		-particle.speedDropFall*Random::Float() * 3.0f,
		0);

	//particle.velocity = glm::vec3(0, 0, 0);

	//particle.velocity.x = 0.0f;
	//particle.velocity.y = particle.speedYDroplet*Random::Float();
	//particle.velocity.z = 0.0f;

	particle.mass = Random::Float()*0.01f;
	//particle.mass = RAND/2 - RAND/4;
	//Constant mass for the raindrop - below 0.5 so as to be relatively small to the scene
	//particle.mass = 0.2;

	//particle.rot_axis = glm::normalize(glm::vec3(
	//	1 - particle.factorXWind * Random::Float(),
	//	1 - 2 * Random::Float(),
	//	1 - particle.factorZWind * Random::Float()));

	particle.rot_axis = glm::normalize(glm::vec3(0,0,0));

	
	particle.accel = glm::vec3(0.0f, -gg, 0); //gg - gravity force
	//particle.rot_angle = Random::Float() * 360;
	particle.rot_angle = 0.0f;
	
	particle.ColorBegin = p_attributes[index].ColorBegin;
	particle.ColorEnd = p_attributes[index].ColorEnd;

	particle.SizeBegin = p_attributes[index].SizeBegin+p_attributes[index].mass*(Random::Float() - 0.5f);
	particle.SizeEnd = p_attributes[index].SizeEnd;

	particle.life = 1.0f; //mark it alive

	index = --index % p_attributes.size();
}

