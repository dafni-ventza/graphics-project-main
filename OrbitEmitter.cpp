//
// Created by stagakis on 15/12/20.
//

#include "OrbitEmitter.h"
#include <iostream>

#define gg 9.80665f

OrbitEmitter::OrbitEmitter(Drawable *_model, int number, float _radius_min, float _radius_max) : IntParticleEmitter(_model, number), radius_min(_radius_min), radius_max(_radius_max) {
	particle_radius.resize(number_of_particles, 0.0f);
	for (int i = 0; i < number_of_particles; i++) {
		createNewParticle(i);
	}
}

void OrbitEmitter::updateParticles(float time, float dt, glm::vec3 camera_pos) {
	for (int i = 0; i < number_of_particles; i++) {
		auto& p = p_attributes[i];

		p.rot_angle += dt / 10.0f;
	/*	p.position = emitter_pos + glm::vec3(particle_radius[i] *
				sin(2*p.rot_angle), 0.0f, particle_radius[i] *
				cos(2*p.rot_angle));*/

	/*	p.position = emitter_pos + glm::vec3(particle_radius[i] *
			sin(p.rot_angle), 0.0f, particle_radius[i] *
			cos(p.rot_angle));*/
		//p.position = emitter_pos;

		//p.position = glm::vec3(sin(p.rot_angle), 0.0f, cos(p.rot_angle));

		p.position = emitter_pos + glm::vec3(sin(p.rot_angle) * 100, 12.0f, 50);

		p.speedDropFall /= 5000.0f;
		p.velocity = glm::vec3(
			0,
			p.speedDropFall,
			0);
	}
}

void OrbitEmitter::createNewParticle(int index) {
	particleAttributes& particle = p_attributes[index];

	particle_radius[index] = RAND * (radius_max - radius_min) +	radius_min;

	particle.rot_angle = 360 * RAND;
	particle.rot_axis = glm::normalize(glm::vec3(1 - RAND, 1 - RAND, 0));
	particle.mass = RAND * 8.0f;
	particle.accel = glm::vec3(0.0f, -gg/100.0f, 0.0f); //gravity force
	particle.life = 1.0f; //mark it alive

}
