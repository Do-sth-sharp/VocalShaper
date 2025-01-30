#include "Track.h"
#include "../misc/VMath.h"
#include "../misc/Renderer.h"
#include "../misc/AudioLock.h"
#include "../uiCallback/UICallback.h"
#include <VSP4.h>

using namespace org::vocalsharp::vocalshaper;

Track::Track(TrackType type,
	const juce::AudioChannelSet& bus)
	: type(type), audioChannels(bus) {
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

	/** Add Sequencer Processor If Need */
	if (type == TrackType::Track) {
		this->sequencerNode = this->addNode(
			std::make_unique<SeqSourceProcessor>(
				[this] { return this->getTrackName(); },
				[this] { return this->getTrackColor(); },
				bus));
	}

	/** Add Mixer Processor If Need */
	this->mixerNode = this->addNode(
		std::make_unique<MixerTrack>(type, bus));

	/** Link Input Channel */
	int channels = this->audioChannels.size();
	if (type == TrackType::Track) {
		/** Link Input to Sequencer Then to Mixer */
		for (int i = 0; i < channels; i++) {
			this->addConnection(
				{ {this->audioInputNode->nodeID, i}, {this->sequencerNode->nodeID, i} });
			this->addConnection(
				{ {this->sequencerNode->nodeID, i}, {this->mixerNode->nodeID, i} });
		}
		this->addConnection(
			{ {this->midiInputNode->nodeID, this->midiChannelIndex},
			{this->sequencerNode->nodeID, this->midiChannelIndex} });
		this->addConnection(
			{ {this->sequencerNode->nodeID, this->midiChannelIndex},
			{this->mixerNode->nodeID, this->midiChannelIndex} });
	}
	else {
		/** Link Input to Mixer */
		for (int i = 0; i < channels; i++) {
			this->addConnection(
				{ {this->audioInputNode->nodeID, i}, {this->mixerNode->nodeID, i} });
		}
		this->addConnection(
			{ {this->midiInputNode->nodeID, this->midiChannelIndex},
			{this->mixerNode->nodeID, this->midiChannelIndex} });
	}

	/** Link Output Channel */
	{
		for (int i = 0; i < channels; i++) {
			this->addConnection(
				{ {this->mixerNode->nodeID, i}, {this->audioOutputNode->nodeID, i} });
		}
		this->addConnection(
			{ {this->mixerNode->nodeID, this->midiChannelIndex},
			{this->audioOutputNode->nodeID, this->midiChannelIndex} });
	}

	/** Set Level Size */
	this->outputLevels.resize(channels);

	/** Default Color */
	this->trackColor = utils::getDefaultColour();
}

Track::~Track() {
	this->setSolo(false);
}

void Track::updateIndex(int index) {
	this->index = index;

	if (auto seq = this->getSequencer()) {
		seq->updateIndex(index);
	}

	if (auto mixer = this->getMixer()) {
		mixer->updateIndex(index);
	}

	/** Callback */
	/*UICallbackAPI<int, int>::invoke(
		UICallbackType::TrackInfoChanged, static_cast<int>(this->type), index);*/
}

SeqSourceProcessor* Track::getSequencer() const {
	if (!this->sequencerNode) { return nullptr; }
	return dynamic_cast<SeqSourceProcessor*>(
		this->sequencerNode->getProcessor());
}

MixerTrack* Track::getMixer() const {
	if (!this->mixerNode) { return nullptr; }
	return dynamic_cast<MixerTrack*>(
		this->mixerNode->getProcessor());
}

const juce::AudioChannelSet& Track::getAudioChannelSet() const {
	return this->audioChannels;
}

void Track::setTrackName(const juce::String& name) {
	this->trackName = name;

	/** ARA Change */
	if (auto seq = this->getSequencer()) {
		seq->syncARATrackInfo();
	}

	/** Callback */
	UICallbackAPI<int, int>::invoke(
		UICallbackType::TrackInfoChanged, static_cast<int>(this->type), this->index);
}

const juce::String Track::getTrackName() const {
	return this->trackName;
}

void Track::setTrackColor(const juce::Colour& color) {
	this->trackColor = color;

	/** ARA Change */
	if (auto seq = this->getSequencer()) {
		seq->syncARATrackInfo();
	}

	/** Callback */
	UICallbackAPI<int, int>::invoke(
		UICallbackType::TrackInfoChanged, static_cast<int>(this->type), this->index);
}

const juce::Colour Track::getTrackColor() const {
	return this->trackColor;
}

void Track::setMute(bool mute) {
	this->isMute = mute;

	/** Callback */
	UICallbackAPI<int, int>::invoke(
		UICallbackType::TrackMuteSoloChanged, static_cast<int>(this->type), this->index);
}

bool Track::getMute() const {
	return this->isMute;
}

void Track::setSolo(bool solo) {
	if (this->type != TrackType::Track) { return; }

	bool shouldChange = (this->isSolo != solo);

	this->isSolo = solo;

	/** Global Solo Count */
	if (shouldChange) {
		if (solo) {
			utils::increaseSoloCount();
		}
		else {
			utils::decreaseSoloCount();
		}
	}

	/** Callback */
	UICallbackAPI<int, int>::invoke(
		UICallbackType::TrackMuteSoloChanged, static_cast<int>(this->type), this->index);
}

bool Track::getSolo() const {
	if (this->type != TrackType::Track) { return false; }
	return this->isSolo;
}

bool Track::getEquivalentMute() const {
	if (this->type != TrackType::Track) { return this->isMute; }

	if (utils::shouldSolo()) {
		return !this->isSolo;
	}
	return this->isMute;
}

bool Track::addAdditionalAudioBus() {
	/** Can't Add Bus To Normal Track */
	if (this->type == TrackType::Track) { return false; }

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
	if (auto ptrMixer = this->getMixer()) {
		jassert(ptrMixer->addAdditionalAudioBus());
	}

	/** Connect Bus To Plugin Dock */
	for (int i = oldNum; i < newNum; i++) {
		this->addConnection({ {this->audioInputNode->nodeID, i},
			{this->mixerNode->nodeID, i} });
		this->addConnection({ {this->mixerNode->nodeID, i},
			{this->audioOutputNode->nodeID, i} });
	}

	return true;
}

bool Track::removeAdditionalAudioBus() {
	/** Can't Add Bus To Normal Track */
	if (this->type == TrackType::Track) { return false; }

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
	if (auto ptrMixer = this->getMixer()) {
		jassert(ptrMixer->removeAdditionalAudioBus());
	}

	/** Auto Remove Connection */
	this->removeIllegalConnections();

	return true;
}

int Track::getAdditionalAudioBusNum() const {
	if (this->audioChannels.size() <= 0) { return 0; }
	return this->getTotalNumInputChannels() / this->audioChannels.size() - 1;
}

const juce::Array<float> Track::getOutputLevels() const {
	juce::ScopedReadLock locker(audioLock::getLevelMeterLock());
	return this->outputLevels;
}

void Track::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessorGraph::setPlayHead(newPlayHead);

	if (auto seq = this->getSequencer()) {
		seq->setPlayHead(newPlayHead);
	}

	if (auto mixer = this->getMixer()) {
		mixer->setPlayHead(newPlayHead);
	}
}

double Track::getTailLengthSeconds() const {
	if (auto seq = this->getSequencer()) {
		return seq->getTailLengthSeconds();
	}
	return 0;
}

void Track::clearGraph() {
	while (this->getAdditionalAudioBusNum() > 0) {
		this->removeAdditionalAudioBus();
	}

	if (auto seq = this->getSequencer()) {
		seq->clearGraph();
	}

	if (auto mixer = this->getMixer()) {
		mixer->clearGraph();
	}
}

bool Track::parse(
	const google::protobuf::Message* data,
	const ParseConfig& config) {
	auto mes = dynamic_cast<const vsp4::Track*>(data);
	if (!mes) { return false; }

	this->clearGraph();

	if (auto sequencer = this->getSequencer()) {
		auto& seqTrack = mes->seqtrack();
		if (!sequencer->parse(&seqTrack, config)) {
			return false;
		}
	}
	if (auto mixer = this->getMixer()) {
		auto& mixerTrack = mes->mixertrack();
		if (!mixer->parse(&mixerTrack, config)) {
			return false;
		}
	}

	auto& info = mes->info();
	this->setTrackName(info.name());
	this->setTrackColor(juce::Colour{ info.color() });

	uint32_t additionalBusNum = mes->additionalbuses();
	for (int i = 0; i < additionalBusNum; i++) {
		this->addAdditionalAudioBus();
	}

	this->setMute(mes->mute());
	this->setSolo(mes->solo());

	return true;
}

std::unique_ptr<google::protobuf::Message> Track::serialize(
	const SerializeConfig& config) const {
	auto mes = std::make_unique<vsp4::Track>();

	mes->set_type(static_cast<vsp4::TrackType>(this->type));
	mes->set_bus(static_cast<vsp4::BusType>(utils::getBusType(this->audioChannels)));
	auto info = mes->mutable_info();
	info->set_name(this->getTrackName().toStdString());
	info->set_color(this->getTrackColor().getARGB());
	mes->set_additionalbuses(this->getAdditionalAudioBusNum());

	mes->set_mute(this->getMute());
	mes->set_solo(this->getSolo());

	if (auto sequencer = this->getSequencer()) {
		auto seqTrack = sequencer->serialize(config);
		if (!dynamic_cast<vsp4::SeqTrack*>(seqTrack.get())) { return nullptr; }
		mes->set_allocated_seqtrack(dynamic_cast<vsp4::SeqTrack*>(seqTrack.release()));
	}
	if (auto mixer = this->getMixer()) {
		auto mixerTrack = mixer->serialize(config);
		if (!dynamic_cast<vsp4::MixerTrack*>(mixerTrack.get())) { return nullptr; }
		mes->set_allocated_mixertrack(dynamic_cast<vsp4::MixerTrack*>(mixerTrack.release()));
	}

	return mes;
}

bool Track::canAddBus(bool isInput) const {
	return isInput && (this->type != TrackType::Track);
}

bool Track::canRemoveBus(bool isInput) const {
	return isInput && (this->type != TrackType::Track);
}

void Track::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Buffer Is Empty */
	if (buffer.getNumChannels() <= 0) { return; }
	if (buffer.getNumSamples() <= 0) { return; }

	/** Process Current Graph */
	this->AudioProcessorGraph::processBlock(buffer, midiMessages);

	/** Process Mute */
	if (this->getEquivalentMute()) {
		vMath::zeroAllAudioData(buffer);
	}

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

