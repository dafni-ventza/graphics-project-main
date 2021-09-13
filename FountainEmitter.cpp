#include "FountainEmitter.h"

#include "common/Random.h"
#include <iostream>
#include <algorithm>

#define gg 9.80665f

FountainEmitter::FountainEmitter(Drawable *_model, int number) : IntParticleEmitter(_model, number) {}
float speedYDroplet = 26.0f;

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

		if (particle.position.y < emitter_pos.y - 80000.0f || particle.life == 0.0f || checkForCollision(particle)) {
			createNewParticle(i);
		}

		if (particle.position.y > height_threshold)
			createNewParticle(i);


		//float val = neighborhoodParticles(particle, modalVertices, index);
		

		particle.accel = glm::vec3(-particle.position.x, 0.0f, 0.0f); //gravity force

		//particle.rot_angle += 90*dt; 

		particle.position = particle.position + particle.velocity*dt + particle.accel*(dt*dt)*0.5f;
		particle.velocity = particle.velocity + particle.accel*dt;

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

float neighborhoodParticles(particleAttributes& particle, std::vector<glm::vec3> modalVertices,int index) {
	//finding the center of the metaball in a less expensive way
	glm::vec3 distance = glm::vec3(0, 0, 0);
	distance += particle.position - particle.dist_from_camera;
	glm::vec3 fallOffCurve = 1.0f / (distance*distance*distance*distance);

	float r = particle.dist_from_camera;
	float value, distanceSquared;
	float rSquared = r * r;

	//comparing the distance between the particle and the vertex modal particular coordinate in worldpace through modalVertices matrix
	distanceSquared = ((modalVertices[index].x - particle.position.x)*(modalVertices[index].x - particle.position.x))
		+ ((modalVertices[index].y - particle.position.y)*(modalVertices[index].y - particle.position.y))
		+ ((modalVertices[index].z - particle.position.z)*(modalVertices[index].z - particle.position.z));

	distanceSquared = distanceSquared / rSquared;

	//optimizing the result
	value = distanceSquared * distanceSquared - distanceSquared + 0.25f;


	if (distanceSquared >= 0.05f) value = 0.0f;
	return value;
}


void FountainEmitter::createNewParticle(int index) {
	particleAttributes & particle = p_attributes[index];

	//p.position = emitter_pos + glm::vec3(sin(p.rot_angle) * 100, 12.0f, 50); //cloud-orbit emitter
	//emitter_pos = glm::vec3(0.0f, 4.0f, 0.0f); //emitter
	particle.position = emitter_pos + glm::vec3(230 - Random::Float() * 200, 10.0f, 260 - Random::Float() * 250);
	particle.velocity = glm::vec3(5 - Random::Float() * 10, -speedYDroplet, 5 - Random::Float() * 10)*4.0f;

	particle.mass = Random::Float() * 0.088f;
	particle.rot_axis = glm::normalize(glm::vec3(0, 0, 0));
	particle.accel = glm::vec3(0.0f, -gg, 0.0f); //gravity force
	particle.rot_angle = Random::Float() * 360;
	particle.life = 1.0f; //mark it alive
}

