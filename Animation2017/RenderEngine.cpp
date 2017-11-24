#include "RenderEngine.h"
#include "Shader.h"
#include "Renderable.h"
#include "Boid.h"
#include "Flock.h"
bool RenderEngine::enabled = true;
std::vector<Flock*> RenderEngine::renderingTargets(0);
Shader* RenderEngine::currentShader = nullptr;
GLuint RenderEngine::projLoc = 0;
GLuint RenderEngine::viewLoc = 0;
GLuint RenderEngine::modelLoc = 0;
int RenderEngine::NumOfCycles = 0;
int RenderEngine::currentCycle = 0;
void RenderEngine::enable() {
	enabled = true;
}

void RenderEngine::disable() {
	enabled = false;
}

bool RenderEngine::isEnabled() {
	return enabled;
}
void RenderEngine::setShader(Shader* s) {
	if (s != currentShader) {
		currentShader = s;
		projLoc = glGetUniformLocation(currentShader->ID, "projection");
		viewLoc = glGetUniformLocation(currentShader->ID, "view");
		modelLoc = glGetUniformLocation(currentShader->ID, "model");

		currentShader->use();
	}
}
Shader* RenderEngine::getShader() {
	return currentShader;
}
void RenderEngine::drawRenderTargets() {
	if (enabled) {
		for (int i = 0; i < renderingTargets.size();i++) {
			if(currentCycle == 0 || currentCycle == renderingTargets[i]->flockID)
			renderingTargets[i]->draw();
		}
		
	}
}

void RenderEngine::addRenderTarget(Flock *renderable) {
	renderingTargets.push_back(renderable);
}

void RenderEngine::cycleFlock() {
	currentCycle = (currentCycle +1) %( NumOfCycles+1);
}