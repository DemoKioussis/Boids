#pragma once

#include <iostream>

#include <GLFW\glfw3.h>

class TimeSystem {


	

	static float elapsedTime;


	static float lastFrameStep;
	static float lastPhysicsStep;

	
	static float frameDeltaTime;
	static float physicsDeltaTime;
	static float timeScale;

	static int fpsCounter;
	static float fpsTracker;
	static int temp;
public:
	static void begin();
	static void update();
	static float physicsCheck();
	static void physicsStep();

	static float frameCheck();
	static void frameStep();
	static void setTimeScale(float t);
	static float getFrameDeltaTime();
	static float getPhysicsDeltaTime();
	static float getTimeScale();
};