#include "AudioCore.h"
#include "Sequencer.h"
#include "Mixer.h"

AudioCore::AudioCore() {
	/** Main Audio Device Manager */
	this->audioDeviceManager = std::make_unique<juce::AudioDeviceManager>();

	/** Main Audio Graph Of The Audio Core */
	this->mainAudioGraph = std::make_unique<juce::AudioProcessorGraph>();

	/** Main Graph Player */
	this->mainGraphPlayer = std::make_unique<juce::AudioProcessorPlayer>();

	/** The Main Audio IO Node Of The Audio Core */
	this->audioInputNode = this->mainAudioGraph->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->mainAudioGraph->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI Input Node Of The Audio Core */
	this->midiInputNode = this->mainAudioGraph->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));

	/** Add Sequencer And Mixer To Audio Graph */
	this->sequencer = this->mainAudioGraph->addNode(std::make_unique<Sequencer>());
	this->mixer = this->mainAudioGraph->addNode(std::make_unique<Mixer>());

	/** Add MIDI Connection */
	this->mainAudioGraph->addConnection(
		{ {this->midiInputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
		{this->sequencer->nodeID, juce::AudioProcessorGraph::midiChannelIndex} });
	this->mainAudioGraph->addConnection(
		{ {this->midiInputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
		{this->mixer->nodeID, juce::AudioProcessorGraph::midiChannelIndex} });
	this->mainAudioGraph->addConnection(
		{ {this->sequencer->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
		{this->mixer->nodeID, juce::AudioProcessorGraph::midiChannelIndex} });

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

}

AudioCore* AudioCore::getInstance() {
	return AudioCore::instance ? AudioCore::instance : (AudioCore::instance = new AudioCore());
}

AudioCore* AudioCore::instance = nullptr;
