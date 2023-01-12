#include "AudioCore.h"
#include "Sequencer.h"
#include "Mixer.h"

AudioCore::AudioCore() {
	/** Main Audio Graph Of The Audio Core */
	this->mainAudioGraph = std::make_unique<juce::AudioProcessorGraph>();

	/** Add Sequencer And Mixer To Audio Graph */
	this->sequencer = this->mainAudioGraph->addNode(std::make_unique<Sequencer>());
	this->mixer = this->mainAudioGraph->addNode(std::make_unique<Mixer>());
}

AudioCore* AudioCore::getInstance() {
	return AudioCore::instance ? AudioCore::instance : (AudioCore::instance = new AudioCore());
}

AudioCore* AudioCore::instance = nullptr;
