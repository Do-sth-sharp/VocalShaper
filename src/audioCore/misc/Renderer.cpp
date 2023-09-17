#include "Renderer.h"

void Renderer::setRendering(bool rendering) {
	this->rendering = rendering;
}

bool Renderer::getRendering() const {
	return this->rendering;
}

void Renderer::writeData(int trackNum, const juce::AudioBuffer<float>& buffer, int offset) {
	if (!this->rendering) { return; }
	/** TODO Write To Audio Collector */
}

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
