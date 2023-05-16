﻿#include "Track.h"

#include "PlayPosition.h"

/** To Fix Symbol Export Error Of juce::dsp::Panner<float> */
#include <juce_dsp/processors/juce_Panner.cpp>

Track::Track(const juce::AudioChannelSet& type)
	: audioChannels(type) {
	/** Set Effects */
	this->setGain(0);
	this->setPan(0);
	this->setSlider(1);

	/** Set Channel Layout */
	juce::AudioProcessorGraph::BusesLayout layout;
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(type.size()));
	layout.outputBuses.add(
		juce::AudioChannelSet::discreteChannels(type.size()));
	this->setBusesLayout(layout);

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

	/** Set Audio Input Node Channel Num */
	juce::AudioProcessorGraph::BusesLayout inputLayout;
	inputLayout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels()));
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** The Plugin Dock Node Of The Track */
	this->pluginDockNode = this->addNode(std::make_unique<PluginDock>(type));

	/** Connect Plugin Dock Node To IO Node */
	int mainBusInputChannels = this->getMainBusNumInputChannels();
	for (int i = 0; i < mainBusInputChannels; i++) {
		this->addConnection(
			{ {this->audioInputNode->nodeID, i}, {this->pluginDockNode->nodeID, i} });
	}
	int mainBusOutputChannels = this->getMainBusNumOutputChannels();
	for (int i = 0; i < mainBusOutputChannels; i++) {
		this->addConnection(
			{ {this->pluginDockNode->nodeID, i}, {this->audioOutputNode->nodeID, i} });
	}
	this->addConnection(
		{ {this->midiInputNode->nodeID, this->midiChannelIndex}, {this->pluginDockNode->nodeID, this->midiChannelIndex} });
}

bool Track::addAdditionalAudioBus() {
	/** Check Channel Num */
	if (this->getTotalNumInputChannels() + this->audioChannels.size() >= juce::AudioProcessorGraph::midiChannelIndex) {
		return false;
	}

	/** Prepare Bus Layout */
	auto layout = this->getBusesLayout();

	/** Set Additional Channel Num */
	layout.inputBuses.clear();
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels() + this->audioChannels.size()));

	/** Set Bus Layout Of Current Graph */
	this->setBusesLayout(layout);

	/** Set Bus Layout Of Input Node */
	juce::AudioProcessorGraph::BusesLayout inputLayout;
	inputLayout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels()));
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** Set Bus Layout Of Plugin Dock Node */
	auto ptrPluginDock = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor());
	if (ptrPluginDock) {
		jassert(ptrPluginDock->addAdditionalAudioBus());
	}

	/** Connect Bus To Plugin Dock */
	int channelNum = this->getTotalNumInputChannels();
	for (int i = channelNum - this->audioChannels.size(); i < channelNum; i++) {
		this->addConnection({ {this->audioInputNode->nodeID, i},
			{this->pluginDockNode->nodeID, i} });
	}

	return true;
}

bool Track::removeAdditionalAudioBus() {
	/** Check Channel Num */
	if (this->getTotalNumInputChannels() - this->audioChannels.size() >= this->audioChannels.size()) {
		return false;
	}

	/** Prepare Bus Layout */
	auto layout = this->getBusesLayout();

	/** Set Additional Channel Num */
	layout.inputBuses.clear();
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels() - this->audioChannels.size()));

	/** Set Bus Layout Of Current Graph */
	this->setBusesLayout(layout);

	/** Set Bus Layout Of Input Node */
	juce::AudioProcessorGraph::BusesLayout inputLayout;
	inputLayout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels()));
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** Set Bus Layout Of Plugin Dock Node */
	auto ptrPluginDock = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor());
	if (ptrPluginDock) {
		jassert(ptrPluginDock->removeAdditionalAudioBus());
	}

	/** Auto Remove Connection */
	this->removeIllegalConnections();

	return true;
}

void Track::setMute(bool mute) {
	this->isMute = mute;
}

bool Track::getMute() const {
	return this->isMute;
}

void Track::setGain(float gain) {
	auto& gainDsp = this->gainAndPanner.get<0>();
	gainDsp.setGainDecibels(gain);
}

float Track::getGain() const {
	auto& gainDsp = this->gainAndPanner.get<0>();
	return gainDsp.getGainDecibels();
}

void Track::setPan(float pan) {
	pan = juce::jlimit(-1.0f, 1.0f, pan);
	this->panValue = pan;

	auto& panDsp = this->gainAndPanner.get<1>();
	panDsp.setPan(pan);
}

float Track::getPan() const {
	return this->panValue;
}

void Track::setSlider(float slider) {
	auto& sliderDsp = this->slider.get<0>();
	sliderDsp.setGainLinear(slider);
}

float Track::getSlider() const {
	auto& sliderDsp = this->slider.get<0>();
	return sliderDsp.getGainLinear();
}

const juce::AudioChannelSet& Track::getAudioChannelSet() const {
	return this->audioChannels;
}

PluginDock* Track::getPluginDock() const {
	return dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor());
}
                                  
void Track::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	if (sampleRate <= 0 || maximumExpectedSamplesPerBlock <= 0) {
		return;
	}

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

	/** Prepare Plugins */
	auto pluginDock = this->getPluginDock();
	if (pluginDock) {
		pluginDock->setPlayHead(PlayPosition::getInstance());
		pluginDock->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
	}
}

bool Track::canAddBus(bool isInput) const {
	return isInput;
}

bool Track::canRemoveBus(bool isInput) const {
	return isInput;
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
