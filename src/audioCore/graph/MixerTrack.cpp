#include "MixerTrack.h"

#include "../uiCallback/UICallback.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

MixerTrack::MixerTrack(TrackType type,
	const juce::AudioChannelSet& bus)
	: type(type), audioChannels(bus) {
	/** Set Effects */
	this->setGain(0);
	this->setPan(0);
	this->setFader(1);

	/** Set Channel Layout */
	juce::AudioProcessorGraph::BusesLayout layout;
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(bus.size()));
	layout.outputBuses.add(
		juce::AudioChannelSet::discreteChannels(bus.size()));
	this->setBusesLayout(layout);

	/** The Main Audio IO Node Of The Track */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI IO Node Of The Track */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));
	this->midiOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode));

	/** Set Audio IO Node Channel Num */
	this->audioInputNode->getProcessor()->setBusesLayout(layout);
	this->audioOutputNode->getProcessor()->setBusesLayout(layout);

	/** The Plugin Dock Node Of The Track */
	this->pluginDockNode = this->addNode(std::make_unique<PluginDock>(bus));

	/** Connect Plugin Dock Node To IO Node */
	int mainBusInputChannels = this->audioChannels.size();
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

	/** Connect MIDI IO Node */
	this->addConnection(
		{ {this->midiInputNode->nodeID, this->midiChannelIndex}, {this->midiOutputNode->nodeID, this->midiChannelIndex} });
}

void MixerTrack::updateIndex(int index) {
	this->index = index;

	if (auto dock = this->getPluginDock()) {
		dock->updateIndex((int)this->type, index);
	}
}

bool MixerTrack::addAdditionalAudioBus() {
	/** Check Channel Num */
	int oldNum = this->getTotalNumInputChannels();
	if (oldNum + this->audioChannels.size() >= juce::AudioProcessorGraph::midiChannelIndex) {
		return false;
	}

	/** Prepare Bus Layout */
	auto layout = this->getBusesLayout();

	/** Set Additional Channel Num */
	layout.inputBuses.clear();
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(oldNum + this->audioChannels.size()));
	layout.outputBuses.clear();
	layout.outputBuses.add(
		juce::AudioChannelSet::discreteChannels(oldNum + this->audioChannels.size()));

	/** Set Bus Layout Of Current Graph */
	this->setBusesLayout(layout);

	/** Set Bus Layout Of Input Node */
	int newNum = this->getTotalNumInputChannels();
	this->audioInputNode->getProcessor()->setBusesLayout(layout);
	this->audioOutputNode->getProcessor()->setBusesLayout(layout);

	/** Set Bus Layout Of Plugin Dock Node */
	if (auto ptrPluginDock = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor())) {
		jassert(ptrPluginDock->addAdditionalAudioBus());
	}

	/** Connect Bus To Plugin Dock */
	for (int i = oldNum; i < newNum; i++) {
		this->addConnection({ {this->audioInputNode->nodeID, i},
			{this->pluginDockNode->nodeID, i} });
		this->addConnection({ {this->pluginDockNode->nodeID, i},
			{this->audioOutputNode->nodeID, i} });
	}

	return true;
}

bool MixerTrack::removeAdditionalAudioBus() {
	/** Check Channel Num */
	int oldNum = this->getTotalNumInputChannels();
	if (oldNum - this->audioChannels.size() < this->audioChannels.size()) {
		return false;
	}

	/** Prepare Bus Layout */
	auto layout = this->getBusesLayout();

	/** Set Additional Channel Num */
	layout.inputBuses.clear();
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(oldNum - this->audioChannels.size()));
	layout.inputBuses.clear();
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(oldNum - this->audioChannels.size()));

	/** Set Bus Layout Of Current Graph */
	this->setBusesLayout(layout);

	/** Set Bus Layout Of Input Node */
	this->audioInputNode->getProcessor()->setBusesLayout(layout);
	this->audioOutputNode->getProcessor()->setBusesLayout(layout);

	/** Set Bus Layout Of Plugin Dock Node */
	if (auto ptrPluginDock = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor())) {
		jassert(ptrPluginDock->removeAdditionalAudioBus());
	}

	/** Auto Remove Connection */
	this->removeIllegalConnections();

	return true;
}

int MixerTrack::getAdditionalAudioBusNum() const {
	if (this->audioChannels.size() <= 0) { return 0; }
	return this->getTotalNumInputChannels() / this->audioChannels.size() - 1;
}

void MixerTrack::setGain(float gain) {
	auto& gainDsp = this->gainAndPanner.get<0>();
	gainDsp.setGainDecibels(gain);

	/** Callback */
	UICallbackAPI<int, int>::invoke(UICallbackType::TrackGainChanged, (int)this->type, this->index);
}

float MixerTrack::getGain() const {
	auto& gainDsp = this->gainAndPanner.get<0>();
	return gainDsp.getGainDecibels();
}

void MixerTrack::setPan(float pan) {
	pan = juce::jlimit(-1.0f, 1.0f, pan);
	this->panValue = pan;

	auto& panDsp = this->gainAndPanner.get<1>();
	panDsp.setPan(pan);

	/** Callback */
	UICallbackAPI<int, int>::invoke(UICallbackType::TrackPanChanged, (int)this->type, this->index);
}

float MixerTrack::getPan() const {
	return this->panValue;
}

void MixerTrack::setFader(float fader) {
	auto& sliderDsp = this->fader.get<0>();
	sliderDsp.setGainLinear(fader);

	/** Callback */
	UICallbackAPI<int, int>::invoke(UICallbackType::TrackFaderChanged, (int)this->type, this->index);
}

float MixerTrack::getFader() const {
	auto& sliderDsp = this->fader.get<0>();
	return sliderDsp.getGainLinear();
}

const juce::AudioChannelSet& MixerTrack::getAudioChannelSet() const {
	return this->audioChannels;
}

PluginDock* MixerTrack::getPluginDock() const {
	return dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor());
}

void MixerTrack::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	if (sampleRate <= 0 || maximumExpectedSamplesPerBlock <= 0) {
		return;
	}

	if (this->audioChannels.size() > 0) {
		/** Prepare Gain And Panner */
		this->gainAndPanner.prepare(juce::dsp::ProcessSpec(
			sampleRate, maximumExpectedSamplesPerBlock,
			this->audioChannels.size()
		));

		/** Prepare Slider */
		this->fader.prepare(juce::dsp::ProcessSpec(
			sampleRate, maximumExpectedSamplesPerBlock,
			this->audioChannels.size()
		));
	}

	/** Prepare Current Graph */
	this->AudioProcessorGraph::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void MixerTrack::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessorGraph::setPlayHead(newPlayHead);

	/** Plugins */
	if (auto pluginDock = this->getPluginDock()) {
		pluginDock->setPlayHead(newPlayHead);
	}
}

void MixerTrack::clearGraph() {
	if (auto plugins = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor())) {
		plugins->clearGraph();
	}

	while (this->getAdditionalAudioBusNum() > 0) {
		this->removeAdditionalAudioBus();
	}

	this->setGain(0);
	this->setPan(0);
	this->setFader(1);
}

bool MixerTrack::parse(
	const google::protobuf::Message* data,
	const ParseConfig& config) {
	auto mes = dynamic_cast<const vsp4::MixerTrack*>(data);
	if (!mes) { return false; }

	this->clearGraph();

	auto& plugins = mes->effects();
	if (!dynamic_cast<PluginDock*>(
		this->pluginDockNode->getProcessor())->parse(&plugins, config)) {
		return false;
	}

	this->setGain(mes->gain());
	this->setPan(mes->panner());
	this->setFader(mes->fader());

	return true;
}

std::unique_ptr<google::protobuf::Message> MixerTrack::serialize(
	const SerializeConfig& config) const {
	auto mes = std::make_unique<vsp4::MixerTrack>();

	auto plugins = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor())->serialize(config);
	if (!dynamic_cast<vsp4::PluginDock*>(plugins.get())) { return nullptr; }
	mes->set_allocated_effects(dynamic_cast<vsp4::PluginDock*>(plugins.release()));

	mes->set_gain(this->getGain());
	mes->set_panner(this->getPan());
	mes->set_fader(this->getFader());

	return mes;
}

bool MixerTrack::canAddBus(bool isInput) const {
	return isInput;
}

bool MixerTrack::canRemoveBus(bool isInput) const {
	return isInput;
}

void MixerTrack::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Buffer Is Empty */
	if (buffer.getNumChannels() <= 0) { return; }
	if (buffer.getNumSamples() <= 0) { return; }
	
	/** Process Gain And Panner */
	int mainChannels = this->audioChannels.size();
	auto block = juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(
		0, mainChannels);
	this->gainAndPanner.process(juce::dsp::ProcessContextReplacing<float>(block));

	/** Process Current Graph */
	this->AudioProcessorGraph::processBlock(buffer, midiMessages);

	/** Process Fader */
	this->fader.process(juce::dsp::ProcessContextReplacing<float>(block));
}
