#include "Track.h"

#include "../misc/Renderer.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../uiCallback/UICallback.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

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

	/** The Main MIDI IO Node Of The Track */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));
	this->midiOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode));

	/** Set Audio Input Node Channel Num */
	juce::AudioProcessorGraph::BusesLayout inputLayout;
	inputLayout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels()));
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** The Plugin Dock Node Of The Track */
	this->pluginDockNode = this->addNode(std::make_unique<PluginDock>(type));

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

	/** Set Level Size */
	this->outputLevels.resize(mainBusOutputChannels);

	/** Default Color */
	this->trackColor = utils::getDefaultColour();
}

void Track::updateIndex(int index) {
	this->index = index;

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, index);
}

bool Track::addAdditionalAudioBus() {
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

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, this->index);

	return true;
}

bool Track::removeAdditionalAudioBus() {
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

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, this->index);

	return true;
}

int Track::getAdditionalAudioBusNum() const {
	if (this->audioChannels.size() <= 0) { return 0; }
	return this->getTotalNumInputChannels() / this->audioChannels.size() - 1;
}

void Track::setMute(bool mute) {
	this->isMute = mute;

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackMuteChanged, this->index);
}

bool Track::getMute() const {
	return this->isMute;
}

void Track::setGain(float gain) {
	auto& gainDsp = this->gainAndPanner.get<0>();
	gainDsp.setGainDecibels(gain);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackGainChanged, this->index);
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

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackPanChanged, this->index);
}

float Track::getPan() const {
	return this->panValue;
}

void Track::setSlider(float slider) {
	auto& sliderDsp = this->slider.get<0>();
	sliderDsp.setGainLinear(slider);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackFaderChanged, this->index);
}

float Track::getSlider() const {
	auto& sliderDsp = this->slider.get<0>();
	return sliderDsp.getGainLinear();
}

void Track::setTrackName(const juce::String& name) {
	this->trackName = name;

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, this->index);
}

const juce::String Track::getTrackName() const {
	return this->trackName;
}

void Track::setTrackColor(const juce::Colour& color) {
	this->trackColor = color;

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, this->index);
}

const juce::Colour Track::getTrackColor() const {
	return this->trackColor;
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

	if (this->audioChannels.size() > 0) {
		/** Prepare Gain And Panner */
		this->gainAndPanner.prepare(juce::dsp::ProcessSpec(
			sampleRate, maximumExpectedSamplesPerBlock,
			this->audioChannels.size()
		));

		/** Prepare Slider */
		this->slider.prepare(juce::dsp::ProcessSpec(
			sampleRate, maximumExpectedSamplesPerBlock,
			this->audioChannels.size()
		));
	}

	/** Prepare Current Graph */
	this->AudioProcessorGraph::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void Track::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessorGraph::setPlayHead(newPlayHead);

	/** Plugins */
	if (auto pluginDock = this->getPluginDock()) {
		pluginDock->setPlayHead(newPlayHead);
	}
}

void Track::clearGraph() {
	auto plugins = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor());
	if (plugins) {
		plugins->clearGraph();
	}

	while (this->getAdditionalAudioBusNum() > 0) {
		this->removeAdditionalAudioBus();
	}

	this->setTrackName(juce::String{});
	this->setTrackColor(juce::Colour{});
	this->setMute(false);
	this->setGain(0);
	this->setPan(0);
	this->setSlider(1);
}

const juce::Array<float> Track::getOutputLevels() const {
	juce::ScopedReadLock locker(audioLock::getLevelMeterLock());
	return this->outputLevels;
}

bool Track::parse(
	const google::protobuf::Message* data,
	const ParseConfig& config) {
	auto mes = dynamic_cast<const vsp4::MixerTrack*>(data);
	if (!mes) { return false; }

	this->clearGraph();

	auto& info = mes->info();
	this->setTrackName(info.name());
	this->setTrackColor(juce::Colour{ info.color() });

	uint32_t additionalBusNum = mes->additionalbuses();
	for (int i = 0; i < additionalBusNum; i++) {
		this->addAdditionalAudioBus();
	}

	auto& plugins = mes->effects();
	if (!dynamic_cast<PluginDock*>(
		this->pluginDockNode->getProcessor())->parse(&plugins, config)) {
		return false;
	}

	this->setMute(mes->muted());
	this->setGain(mes->gain());
	this->setPan(mes->panner());
	this->setSlider(mes->slider());

	return true;
}

std::unique_ptr<google::protobuf::Message> Track::serialize(
	const SerializeConfig& config) const {
	auto mes = std::make_unique<vsp4::MixerTrack>();

	mes->set_type(static_cast<vsp4::TrackType>(utils::getTrackType(this->audioChannels)));
	auto info = mes->mutable_info();
	info->set_name(this->getTrackName().toStdString());
	info->set_color(this->getTrackColor().getARGB());
	mes->set_additionalbuses(this->getAdditionalAudioBusNum());

	auto plugins = dynamic_cast<PluginDock*>(this->pluginDockNode->getProcessor())->serialize(config);
	if (!dynamic_cast<vsp4::PluginDock*>(plugins.get())) { return nullptr; }
	mes->set_allocated_effects(dynamic_cast<vsp4::PluginDock*>(plugins.release()));

	mes->set_muted(this->getMute());
	mes->set_gain(this->getGain());
	mes->set_panner(this->getPan());
	mes->set_slider(this->getSlider());

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

bool Track::canAddBus(bool isInput) const {
	return isInput;
}

bool Track::canRemoveBus(bool isInput) const {
	return isInput;
}

void Track::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
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

	/** Process Mute */
	if (this->isMute) {
		block.fill(0);
	}

	/** Process Slider */
	this->slider.process(juce::dsp::ProcessContextReplacing<float>(block));

	/** Update Level Meter */
	for (int i = 0; i < buffer.getNumChannels() && i < this->outputLevels.size(); i++) {
		this->outputLevels.getReference(i) =
			buffer.getRMSLevel(i, 0, buffer.getNumSamples());
	}

	/** Render */
	if (Renderer::getInstance()->getRendering()) {
		if (auto playHead = this->getPlayHead()) {
			auto pos = playHead->getPosition();
			int64_t offset = pos->getTimeInSamples().orFallback(0);

			Renderer::getInstance()->writeData(this, buffer, offset);
		}
	}
}
