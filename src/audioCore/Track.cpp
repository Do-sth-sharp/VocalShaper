#include "Track.h"
#include "PluginDock.h"

/** To Fix Symbol Export Error Of juce::dsp::Panner<float> */
#include <juce_dsp/processors/juce_Panner.cpp>

Track::Track(const juce::AudioChannelSet& type)
	: audioChannels(type) {
	/** Set Channel Layout */
	this->setChannelLayoutOfBus(true, 0, type);
	this->setChannelLayoutOfBus(false, 0, type);

	/** The Main Audio IO Node Of The Track */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI Input Node Of The Track */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));

	/** The Plugin Dock Node Of The Track */
	this->pluginDockNode = this->addNode(std::make_unique<PluginDock>(type));

	/** Connect Plugin Dock Node To IO Node */
	int mainBusChannels = this->getMainBusNumInputChannels();
	for (int i = 0; i < mainBusChannels; i++) {
		this->addConnection(
			{ {this->audioInputNode->nodeID, i}, {this->pluginDockNode->nodeID, i} });
		this->addConnection(
			{ {this->pluginDockNode->nodeID, i}, {this->audioOutputNode->nodeID, i} });
	}
	this->addConnection(
		{ {this->midiInputNode->nodeID, this->midiChannelIndex}, {this->pluginDockNode->nodeID, this->midiChannelIndex} });
}

void Track::addAdditionalAudioBus() {
	/** Prepare Bus Layout */
	auto layout = this->getBusesLayout();
	layout.inputBuses.add(this->audioChannels);

	/** Set Bus Layout Of Current Graph */
	this->setBusesLayout(layout);

	/** Set Bus Layout Of Input Node */
	juce::AudioProcessorGraph::BusesLayout inputLayout = layout;
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** Set Bus Layout Of Plugin Dock Node */
	auto ptrPluginDock = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor());
	if (ptrPluginDock) {
		ptrPluginDock->addAdditionalAudioBus();
	}

	/** Connect Bus To Plugin Dock */
	int channelNum = this->getTotalNumInputChannels();
	for (int i = channelNum - this->audioChannels.size(); i < channelNum; i++) {
		this->addConnection({ {this->audioInputNode->nodeID, i},
			{this->pluginDockNode->nodeID, i} });
	}
}

void Track::removeAdditionalAudioBus() {
	/** Check Has Additional Bus */
	auto layout = this->getBusesLayout();
	if (layout.inputBuses.size() > 1) {
		/** Prepare Bus Layout */
		layout.inputBuses.removeLast();

		/** Set Bus Layout Of Current Graph */
		this->setBusesLayout(layout);

		/** Set Bus Layout Of Input Node */
		juce::AudioProcessorGraph::BusesLayout inputLayout = layout;
		inputLayout.outputBuses = inputLayout.inputBuses;
		this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

		/** Set Bus Layout Of Plugin Dock Node */
		auto ptrPluginDock = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor());
		if (ptrPluginDock) {
			ptrPluginDock->removeAdditionalAudioBus();
		}

		/** Auto Remove Connection */
		this->removeIllegalConnections();
	}
}

void Track::setMute(bool mute) {
	this->isMute = mute;
}

bool Track::getMute() const {
	return this->isMute;
}

const juce::AudioChannelSet& Track::getAudioChannelSet() const {
	return this->audioChannels;
}
                                  
void Track::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** Prepare Gain And Panner */
	this->gainAndPanner.prepare(juce::dsp::ProcessSpec(
		sampleRate, maximumExpectedSamplesPerBlock,
		this->getMainBusNumInputChannels()
	));

	/** Prepare Slider */
	this->slider.prepare(juce::dsp::ProcessSpec(
		sampleRate, maximumExpectedSamplesPerBlock,
		this->getMainBusNumInputChannels()
	));

	/** Prepare Current Graph */
	this->AudioProcessorGraph::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void Track::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Process Gain And Panner */
	auto block = juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(
		0, this->getMainBusNumInputChannels());
	this->gainAndPanner.process(juce::dsp::ProcessContextReplacing<float>(block));

	/** Process Current Graph */
	this->AudioProcessorGraph::processBlock(buffer, midiMessages);

	/** Process Mute */
	if (this->isMute) {
		block.fill(0);
	}

	/** Process Slider */
	this->slider.process(juce::dsp::ProcessContextReplacing<float>(block));
}
