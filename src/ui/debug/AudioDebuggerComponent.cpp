#include "AudioDebuggerComponent.h"
#include "../../audioCore/AC_API.h"

AudioDebuggerComponent::AudioDebuggerComponent()
	: FlowComponent(TRANS("Audio Debugger")) {
	this->comp = quickAPI::getAudioDebugger();
	this->addAndMakeVisible(this->comp);
}

void AudioDebuggerComponent::resized() {
	if (this->comp) {
		this->comp->setBounds(this->getLocalBounds());
	}
}
