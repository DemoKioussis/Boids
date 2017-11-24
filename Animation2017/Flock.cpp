#include "Boid.h"
#include "Flock.h"

// How large each boid is
#define MODEL_SCALE 1.0f

using namespace glm;

int Flock::flockCount = 1;

Flock::Flock(int flockSize, glm::vec3 origin, float radius, float bRad, float bForce,glm::vec3 color, 
	cl::CommandQueue* q, cl::Context * c,cl::Program *p) {

	queue = q;
	context = c;
	computeNextStep = cl::Kernel(*p, "computeNextState");
	doNextStep = cl::Kernel(*p, "doNextStep");


	flockState = false;
	stateCounter = flockSize;
	if (flockCount == 0) {
		srand(time(NULL));
	}
	flockID = flockCount++;

	center = origin;
	for (int i = 0; i < flockSize;i++) {

		vec3 randomPositionVector((float(rand()) / float(RAND_MAX)) - 0.5f, (float(rand()) / float(RAND_MAX)) - 0.5f, (float(rand()) / float(RAND_MAX)) - 0.5f);
		vec3 randomFrontVector((float(rand()) / float(RAND_MAX)) - 0.5f, (float(rand()) / float(RAND_MAX)) - 0.5f, (float(rand()) / float(RAND_MAX)) - 0.5f);
		float randomFloat = ((float)rand() / RAND_MAX)*radius;
		vec3 translation = normalize(randomPositionVector)*randomFloat;
		Boid* b = new Boid(center + translation, normalize(randomFrontVector),color);
		b->flock = this;
		flock.push_back(b);
	}
	boundaryRadius = bRad;
	boundaryForce = bForce;

	makeMesh(color);
}
Flock::~Flock() {
	flock.clear();
}

void Flock::makeMesh(glm::vec3 color) {

#pragma region MeshStuff

	glm::vec3 forward(0, 0, 5 * MODEL_SCALE);
	glm::vec3 downLeft(-MODEL_SCALE, -MODEL_SCALE, 0);
	glm::vec3 downRight(MODEL_SCALE, -MODEL_SCALE, 0);
	glm::vec3 upRight(MODEL_SCALE, MODEL_SCALE, 0);
	glm::vec3 upLeft(-MODEL_SCALE, MODEL_SCALE, 0);

	glm::vec3 A = glm::normalize(glm::cross(forward - downLeft, forward - downRight));
	glm::vec3 B = glm::normalize(glm::cross(forward - downRight, forward - upRight));
	glm::vec3 C = glm::normalize(glm::cross(forward - upRight, forward - upLeft));
	glm::vec3 D = glm::normalize(glm::cross(forward - upLeft,forward - downLeft));

	GLfloat vertices[] = {
		-MODEL_SCALE,-MODEL_SCALE,0,		1.0f, 0.5, 0.3,		 0,0,1	//back left
		,MODEL_SCALE,-MODEL_SCALE,0,		1.0f, 0.5, 0.3,		 0,0,1		//back right
		,MODEL_SCALE,MODEL_SCALE,0,		    1.0f, 0.5, 0.3,		 0,0,1		//front right

		,MODEL_SCALE,MODEL_SCALE,0,		    1.0f, 0.5, 0.3,		 0,0,1		//front right
		,-MODEL_SCALE,MODEL_SCALE,0,		1.0f, 0.5, 0.3,		 0,0,1		//back right
		,-MODEL_SCALE,-MODEL_SCALE,0,		1.0f, 0.5, 0.3,		 0,0,1		//back left

		,- MODEL_SCALE,-MODEL_SCALE,0,		1.0f, 0.5, 0.3,		 A.x,A.y,A.z
		,0,0,5 * MODEL_SCALE,				1.0f, 0.5, 0.3,		 A.x,A.y,A.z
		,MODEL_SCALE,-MODEL_SCALE,0,		1.0f, 0.5, 0.3,		 A.x,A.y,A.z

		,MODEL_SCALE,-MODEL_SCALE,0,		1.0f, 0.5, 0.3,		 B.x,B.y,B.z
		,0,0,5 * MODEL_SCALE,				1.0f, 0.5, 0.3,		 B.x,B.y,B.z
		,MODEL_SCALE,MODEL_SCALE,0,		    1.0f, 0.5, 0.3,		 B.x,B.y,B.z

		,MODEL_SCALE,MODEL_SCALE,0,		    1.0f, 0.5, 0.3,		 C.x,C.y,C.z
		,0,0,5 * MODEL_SCALE,				1.0f, 0.5, 0.3,		 C.x,C.y,C.z
		,-MODEL_SCALE,MODEL_SCALE,0,		    1.0f, 0.5, 0.3,		 C.x,C.y,C.z

		,-MODEL_SCALE,MODEL_SCALE,0,		1.0f, 0.5, 0.3,		 D.x,D.y,D.z
		,0,0,5 * MODEL_SCALE,				1.0f, 0.5, 0.3,		 D.x,D.y,D.z
		,-MODEL_SCALE,-MODEL_SCALE,0,		1.0f, 0.5, 0.3,		 D.x,D.y,D.z
	};

	GLuint indices[] = {  // note that we start from 0!
		0,1,2
		,0,3,2
		,0,4,1
		,1,4,2
		,2,4,3
		,3,4,0
	};

	mesh = new Mesh();
	std::vector<GLfloat> positions(0), colors(0), normals(0);
	std::vector<GLuint> *inds = new std::vector<GLuint>(0);
	for (int i = 0; i < sizeof(vertices) / sizeof(GLfloat); i += 9) {
		positions.push_back(vertices[i]);
		positions.push_back(vertices[i + 1]);
		positions.push_back(vertices[i + 2]);

		colors.push_back(color.x);
		colors.push_back(color.y);
		colors.push_back(color.z);

		normals.push_back(vertices[i + 6]);
		normals.push_back(vertices[i + 7]);
		normals.push_back(vertices[i + 8]);
	}
	for (int i = 0; i < sizeof(indices) / sizeof(GLuint); i++) {
		inds->push_back(indices[i]);
	}

	mesh->setVerticies(positions, colors, normals);
	mesh->setIndices(inds);

	RenderEngine::addRenderTarget(this);
	setDataBuffer();


	glBuffers.push_back(cl::BufferGL(*context, CL_MEM_READ_WRITE, mesh->posBuffer));
	glBuffers.push_back(cl::BufferGL(*context, CL_MEM_READ_WRITE, mesh->velBuffer));
	transferBuffer = cl::Buffer(*context, CL_MEM_READ_WRITE, sizeof(float) * 4 * flock.size());

#pragma endregion
}
void Flock::setDataBuffer() {
	glBindVertexArray(mesh->VAO);
	vector<glm::vec4> posData(flock.size());
	vector<glm::vec4> velData(flock.size());

	for (int i = 0; i < flock.size();i++) {
		posData[i] = glm::vec4(flock[i]->position, 1);
		velData[i] = glm::vec4(flock[i]->velocity, 0);
		mesh->setBufferData(posData, mesh->posBuffer, 3);
		mesh->setBufferData(velData, mesh->velBuffer, 4);
	}

}
void Flock::step() {
	cl::Event ev;
	try {
		glFinish();
		cl_int res = queue->enqueueAcquireGLObjects(&glBuffers, NULL, &ev);
		computeNextStep.setArg(0, glBuffers[0]);
		computeNextStep.setArg(1, glBuffers[1]);
		computeNextStep.setArg(2, transferBuffer);
		queue->enqueueNDRangeKernel(computeNextStep, cl::NullRange, cl::NDRange(flock.size() * 4), cl::NullRange, NULL, NULL);
		ev.wait();

		doNextStep.setArg(0, glBuffers[0]);
		doNextStep.setArg(1, glBuffers[1]);
		doNextStep.setArg(2, transferBuffer);
		queue->enqueueNDRangeKernel(doNextStep, cl::NullRange, cl::NDRange(flock.size() * 4), cl::NullRange, NULL, NULL);
		queue->enqueueReleaseGLObjects(&glBuffers);
		ev.wait();



		if (res != CL_SUCCESS) {
			std::cout << "Failed releasing GL object: " << res << std::endl;
			exit(247);
		}

		//read result C from the device to array C
		// flush opengl commands and wait for object acquisition

		queue->finish();

	}
	catch (...) {
		std::cout << "Problem" << std::endl;
	}
}

void Flock::draw() {
	glBindVertexArray(mesh->VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, sizeof(GLfloat)*mesh->vertices->size(),flock.size());
	glBindVertexArray(0);
}
