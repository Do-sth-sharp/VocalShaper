#include "AudioCore.h"
#include "MainGraph.h"

AudioCore::AudioCore() {
	/** Main Audio Device Manager */
	this->audioDeviceManager = std::make_unique<juce::AudioDeviceManager>();

	/** Main Audio Graph Of The Audio Core */
	this->mainAudioGraph = std::make_unique<MainGraph>();

	/** Main Graph Player */
	this->mainGraphPlayer = std::make_unique<juce::AudioProcessorPlayer>();

	/** Init Audio Device */
	this->initAudioDevice();
}

void AudioCore::initAudioDevice() {
	/** Init With Default Device */
	this->audioDeviceManager->initialiseWithDefaultDevices(0, 2);

	/** Add Main Graph To Main Player */
	this->mainGraphPlayer->setProcessor(this->mainAudioGraph.get());

	/** Add Player To Default Device */
	this->audioDeviceManager->addAudioCallback(this->mainGraphPlayer.get());

	/** Update Audio Buses */
	this->updateAudioBuses();
}

void AudioCore::updateAudioBuses() {
	/** Link Audio Bus To Sequencer And Mixer */
	auto mainGraph = dynamic_cast<MainGraph*>(this->mainAudioGraph.get());
	if (mainGraph) {
		/** Get Input Channel Num */
		auto audioDeviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
		auto inputChannelNum = audioDeviceSetup.inputChannels.toInteger();
		auto outputChannelNum = audioDeviceSetup.outputChannels.toInteger();

		/** Count Buses */
		int inputSteroBusNum = inputChannelNum / 2;
		int inputMonoBusNum = inputChannelNum % 2;
		int outputSteroBusNum = outputChannelNum / 2;
		int outputMonoBusNum = outputChannelNum % 2;

		/** Create Buses Layout */
		juce::AudioProcessorGraph::BusesLayout layout;
		for (int i = 0; i < inputSteroBusNum; i++) {
			layout.inputBuses.add(juce::AudioChannelSet::stereo());
		}
		for (int i = 0; i < inputMonoBusNum; i++) {
			layout.inputBuses.add(juce::AudioChannelSet::mono());
		}
		for (int i = 0; i < outputSteroBusNum; i++) {
			layout.outputBuses.add(juce::AudioChannelSet::stereo());
		}
		for (int i = 0; i < outputMonoBusNum; i++) {
			layout.outputBuses.add(juce::AudioChannelSet::mono());
		}

		/** Set Buses Layout Of Main Graph */
		mainGraph->setAudioLayout(layout);
	}
}

AudioCore* AudioCore::getInstance() {
	return AudioCore::instance ? AudioCore::instance : (AudioCore::instance = new AudioCore());
}

AudioCore* AudioCore::instance = nullptr;
