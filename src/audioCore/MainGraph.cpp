#include "MainGraph.h"
#include "Sequencer.h"
#include "Mixer.h"

MainGraph::MainGraph() {
	/** The Main Audio IO Node */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI Input Node */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));

	/** Add Sequencer And Mixer To Audio Graph */
	this->sequencer = this->addNode(std::make_unique<Sequencer>());
	this->mixer = this->addNode(std::make_unique<Mixer>());

	/** Add MIDI Connection */
	this->addConnection(
		{ {this->midiInputNode->nodeID, this->midiChannelIndex},
		{this->sequencer->nodeID, this->midiChannelIndex} });
	this->addConnection(
		{ {this->midiInputNode->nodeID, this->midiChannelIndex},
		{this->mixer->nodeID, this->midiChannelIndex} });
	this->addConnection(
		{ {this->sequencer->nodeID, this->midiChannelIndex},
		{this->mixer->nodeID, this->midiChannelIndex} });
}

void MainGraph::setAudioLayout(const juce::AudioProcessorGraph::BusesLayout& busLayout) {
	/** Set Layout Of Main Graph */
	this->setBusesLayout(busLayout);

	/** Set Layout Of Input Node */
	juce::AudioProcessorGraph::BusesLayout inputLayout = busLayout;
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** Set Layout Of Output Node */
	juce::AudioProcessorGraph::BusesLayout outputLayout = busLayout;
	outputLayout.inputBuses = outputLayout.outputBuses;
	this->audioOutputNode->getProcessor()->setBusesLayout(outputLayout);

	/** Set Layout Of Sequencer And Mixer */
	auto sequencer = dynamic_cast<Sequencer*>(this->sequencer->getProcessor());
	if (sequencer) {
		sequencer->setInputChannels(busLayout.inputBuses);
	}
	auto mixer = dynamic_cast<Mixer*>(this->mixer->getProcessor());
	if (mixer) {
		mixer->setInputDeviceChannels(busLayout.inputBuses);
		mixer->setOutputChannels(busLayout.outputBuses);
	}

	/** Link Audio Channels Of Sequencer And Mixer */
	this->removeIllegalConnections();

	int inputChannelsNum = busLayout.inputBuses.size();
	int outputChannelsNum = busLayout.outputBuses.size();
	for (int i = 0; i < inputChannelsNum; i++) {
		this->addConnection(
			{ {this->audioInputNode->nodeID, i},
			{this->sequencer->nodeID, i} });
		this->addConnection(
			{ {this->audioInputNode->nodeID, i},
			{this->mixer->nodeID, i} });
	}
	for (int i = 0; i < outputChannelsNum; i++) {
		this->addConnection(
			{ {this->mixer->nodeID, i},
			{this->audioOutputNode->nodeID, i} });
	}
}
