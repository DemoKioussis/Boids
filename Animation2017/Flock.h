#pragma once
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <glm\glm.hpp>

#include "Mesh.h"
#include "RenderEngine.h"
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.hpp>
#include <CL/cl_gl.h>

class Boid;
class Mesh;
class Flock {
	static int flockCount;
public:
	bool flockState;
	int flockID;
	int stateCounter;
	float boundaryRadius;
	float boundaryForce;
	Mesh* mesh;
	std::vector<Boid*> flock;
	std::vector<cl::Memory> glBuffers;
	cl::Buffer transferBuffer;
	cl::Context* context;
	cl::CommandQueue* queue;
	cl::Kernel computeNextStep;
	cl::Kernel doNextStep;
	Flock(int flockSize, glm::vec3 origin, float radius, float boundaryRadius, float boundaryForce, glm::vec3 color,
		cl::CommandQueue* q, cl::Context * c, cl::Program * p);
	~Flock();
	void makeMesh(glm::vec3 color);
	void setDataBuffer();
	glm::vec3 center;

	void step();
	void draw();

};