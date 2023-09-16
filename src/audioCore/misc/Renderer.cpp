#include "Renderer.h"

Renderer* Renderer::getInstance() {
	return Renderer::instance
		? Renderer::instance : (Renderer::instance = new Renderer());
}

void Renderer::releaseInstance() {
	if (Renderer::instance) {
		delete Renderer::instance;
		Renderer::instance = nullptr;
	}
}

Renderer* Renderer::instance = nullptr;
