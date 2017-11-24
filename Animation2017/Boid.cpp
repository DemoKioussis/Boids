#include "Boid.h"
#include "Flock.h"
Boid::Boid(glm::vec3 p,glm::vec3 f, glm::vec3 c){
	position = p;	
	front = f;
	velocity = glm::normalize(front)*MAX_SPEED;
}
