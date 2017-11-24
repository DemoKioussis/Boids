#pragma once


#include <vector>
#include <glad/glad.h>

class Shader;
class Mesh;
class Flock;
class RenderEngine {

public:
	static void addRenderTarget(Flock *renderingTarget);
	static void drawRenderTargets();
	static void enable();
	static void disable();
	static bool isEnabled();
	static void setShader(Shader* s);
	static Shader* getShader();
	static GLuint projLoc;
	static GLuint viewLoc;
	static GLuint modelLoc;
	static void cycleFlock();
	static int NumOfCycles;

private:
	static std::vector<Flock*> renderingTargets;
	static bool enabled;
	static Shader* currentShader;
	static int currentCycle;



};