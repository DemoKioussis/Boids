#pragma once


#include <glm/glm.hpp>



// the max/min speeds of each boids.
#define MAX_SPEED 2.0f
#define MIN_SPEED 0.20f

// these are the weights for each computation. cohesion should
// always be less than separation
#define separationWeight 1.5f
#define alingmentWeight 5.0f
#define cohesionWeight  0.9f

// this is the radius at which each rule can be applied. Separation should
// be lower than cohesion
#define separationRadius 1.0f
#define alingmentRadius 20.0f
#define cohesionRadius 20.0f

// this angle is half the angle of the cone that they can see
#define coneHalfAngle 120.0f

// smoothing affects how smoothly that rotate. smaller means that they will take more simulation
// ticks to rotate, so changing this can affect flocking behaviours
#define smoothing 0.10f

class Flock;
class Boid {
	friend class Flock;
private:

	glm::vec3 velocity;
	glm::vec3 front;
	glm::vec3 position;

public:
	Flock* flock;
	Boid(glm::vec3 p,glm::vec3 f,glm::vec3 c);

	

};