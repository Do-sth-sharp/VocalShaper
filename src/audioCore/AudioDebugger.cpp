#include "AudioDebugger.h"
#include "AudioCore.h"

AudioDebugger::AudioDebugger(AudioCore* parent) 
	: parent(parent){
	this->setOpaque(true);

	this->deviceSelector = std::make_unique<juce::AudioDeviceSelectorComponent>(
		*(parent->audioDeviceManager.get()), 0, 1024, 2, 1024,
		true, true, false, false);
	this->addAndMakeVisible(this->deviceSelector.get());
}

void AudioDebugger::resized() {
	this->deviceSelector->setBounds(this->getLocalBounds());
}

void AudioDebugger::paint(juce::Graphics& g) {
	g.fillAll(
		juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
			juce::ResizableWindow::backgroundColourId));
}

void AudioDebugger::userTriedToCloseWindow() {
	this->setVisible(false);
}
