#include "TimeSystem.h"


float TimeSystem::elapsedTime = 0;


float TimeSystem::lastFrameStep = 0;
float TimeSystem::lastPhysicsStep = 0;


float TimeSystem::frameDeltaTime = 0;
float TimeSystem::physicsDeltaTime = 0;


float TimeSystem::timeScale = 1;

int TimeSystem::temp = 0;

int TimeSystem::fpsCounter = 0;
float TimeSystem::fpsTracker = 0;

void TimeSystem::begin() {

	elapsedTime = glfwGetTime();
	lastFrameStep = elapsedTime;
	lastPhysicsStep = elapsedTime;
	
	frameDeltaTime = 1/30;
	physicsDeltaTime = 1/30;
};

void TimeSystem::update() {

	elapsedTime = glfwGetTime();
	if (elapsedTime - fpsTracker >= 1) {
		std::cout << "Fps: " << fpsCounter<<std::endl;
		fpsCounter = 0;
		fpsTracker = elapsedTime;
	}
};

float TimeSystem::physicsCheck() {
	return elapsedTime - lastPhysicsStep;;
};

void TimeSystem::physicsStep() {
	physicsDeltaTime = elapsedTime - lastPhysicsStep;
	lastPhysicsStep = elapsedTime;
};


float TimeSystem::frameCheck() {
	return elapsedTime - lastFrameStep;
};
void TimeSystem::frameStep() {
	frameDeltaTime = elapsedTime - lastFrameStep;
	lastFrameStep = elapsedTime;
	fpsCounter++;

};

void TimeSystem::setTimeScale(float t) {
	timeScale = t;
};
float TimeSystem::getTimeScale() {
	return timeScale;
}
float TimeSystem::getFrameDeltaTime() {
	return frameDeltaTime;
};
float TimeSystem::getPhysicsDeltaTime() {
	return physicsDeltaTime;
};
