#include "MainGraph.h"

#include "../misc/PlayPosition.h"
#include "../misc/Renderer.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../uiCallback/UICallback.h"
#include "../AudioCore.h"
#include "../Utils.h"
#include "SourceRecordProcessor.h"
#include <VSP4.h>

using namespace org::vocalsharp::vocalshaper;

MainGraph::MainGraph() {
	/** The Main Audio IO Node */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI IO Node */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));
	this->midiOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode));

	/** The Source Recorder Node */
	this->recorder = std::make_unique<SourceRecordProcessor>(this);

	/** Init Bus Layout */
	this->setAudioLayout(0, 2);

	/** Init Master Track */
	this->initMasterTrack();
}

MainGraph::~MainGraph() {
	this->clearGraph();
}

void MainGraph::insertTrack(TrackType type, int index,
	const juce::AudioChannelSet& bus) {
	/** Check Track Type */
	if (type == TrackType::MasterTrack) { return; }

	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Get Track List */
	auto& trackList = (type == TrackType::Track)
		? this->trackList : this->auxTrackList;

	/** Add Node To Graph */
	if (auto ptrNode = this->addNode(std::make_unique<Track>(type, bus))) {
		/** Limit Index */
		if (index < 0 || index > trackList.size()) {
			index = trackList.size();
		}

		/** Add Node To List */
		trackList.insert(index, ptrNode);

		/** Connect Track Audio Output To Master Track */
		if (auto masterNode = this->masterTrack) {
			auto masterNodeID = masterNode->nodeID;
			if (auto masterProcessor = dynamic_cast<Track*>(masterNode->getProcessor())) {
				int channels = std::min(bus.size(), masterProcessor->getAudioChannelSet().size());
				for (int i = 0; i < channels; i++) {
					if (this->addAudioSendLink(ptrNode->nodeID, 0, masterNodeID, i, i)) {
						this->addConnection(
							{ {ptrNode->nodeID, i}, {masterNodeID, i} });
					}
				}
			}
		}

		/** Prepare To Play */
		ptrNode->getProcessor()->setPlayHead(this->getPlayHead());
		ptrNode->getProcessor()->prepareToPlay(this->getSampleRate(), this->getBlockSize());

		/** Set Index */
		dynamic_cast<Track*>(ptrNode->getProcessor())->updateIndex(index);
		for (int i = index + 1; i < trackList.size(); i++) {
			auto node = trackList[i];
			dynamic_cast<Track*>(node->getProcessor())->updateIndex(i);
		}

		/** Callback */
		UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, index);
	}
}

void MainGraph::removeTrack(TrackType type, int index) {
	/** Check Track Type */
	if (type == TrackType::MasterTrack) { return; }

	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Get Track List */
	auto& trackList = (type == TrackType::Track)
		? this->trackList : this->auxTrackList;

	/** Limit Index */
	if (index < 0 || index >= trackList.size()) { return; }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = trackList.removeAndReturn(index);
	auto nodeID = ptrNode->nodeID;

	/** Remove Input And Output Connections */
	this->disconnectTrackLinks(nodeID);
	this->destoryTrackLinkTemps(nodeID);

	/** Remove Node From Graph */
	this->removeNode(nodeID);

	/** Set Index */
	for (int i = index; i < trackList.size(); i++) {
		auto node = trackList[i];
		dynamic_cast<Track*>(node->getProcessor())->updateIndex(i);
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, index);
}

int MainGraph::getTrackNum(TrackType type) const {
	/** Check Track Type */
	if (type == TrackType::MasterTrack) {
		return this->masterTrack ? 1 : 0;
	}

	/** Get Track List */
	auto& trackList = (type == TrackType::Track)
		? this->trackList : this->auxTrackList;

	/** Get Num */
	return trackList.size();
}

Track* MainGraph::getTrackProcessor(TrackType type, int index) const {
	/** Check Track Type */
	if (type == TrackType::MasterTrack) {
		return this->masterTrack
			? dynamic_cast<Track*>(this->masterTrack->getProcessor()) : nullptr;
	}

	/** Get Track List */
	auto& trackList = (type == TrackType::Track)
		? this->trackList : this->auxTrackList;

	/** Check Index */
	if (index < 0 || index >= trackList.size()) {
		return nullptr;
	}

	/** Return Processor */
	return dynamic_cast<Track*>(trackList.getUnchecked(index)->getProcessor());
}

bool MainGraph::addTrackAdditionalAudioBus(TrackType type, int index) {
	/** Check Track Type */
	if (type == TrackType::Track) { return false; }

	/** Get Track Processor */
	if (auto track = this->getTrackProcessor(type, index)) {
		return track->addAdditionalAudioBus();
	}

	return false;
}

bool MainGraph::removeTrackAdditionalAudioBus(TrackType type, int index) {
	/** Check Track Type */
	if (type == TrackType::Track) { return false; }

	/** Get Track Processor */
	if (auto track = this->getTrackProcessor(type, index)) {
		if (!track->removeAdditionalAudioBus()) { return false; }

		/** Remove Illegal Connections */
		auto nodeID = this->getTrackNodeIndex(type, index);
		this->removeIllegalNodeAudioSendInputConnections(
			nodeID, track->getTotalNumInputChannels());
	}

	return false;
}

int MainGraph::getTrackAdditionalAudioBusNum(TrackType type, int index) const {
	/** Check Track Type */
	if (type == TrackType::Track) { return 0; }

	/** Get Track Processor */
	if (auto track = this->getTrackProcessor(type, index)) {
		return track->getAdditionalAudioBusNum();
	}

	return 0;
}

bool MainGraph::connectTrackMIDIInput(TrackType type, int index) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	auto inputNode = this->midiInputNode->nodeID;

	/** Add Link */
	if (this->addMIDIInputLink(trackNode)) {
		this->addConnection({ {inputNode, midiChannelIndex}, {trackNode, midiChannelIndex} });
		return true;
	}

	return false;
}

bool MainGraph::connectTrackAudioInput(
	TrackType type, int index, int inputChannel, int trackChannel) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	auto inputNode = this->audioInputNode->nodeID;

	/** Check Channel Num */
	if (inputChannel >= this->getTotalNumInputChannels()) { return false; }
	if (auto track = this->getTrackProcessor(type, index)) {
		if (trackChannel >= track->getTotalNumInputChannels()) { return false; }
	}
	else {
		return false;
	}

	/** Add Link */
	if (this->addAudioInputLink(trackNode, inputChannel, trackChannel)) {
		this->addConnection({ {inputNode, inputChannel}, {trackNode, trackChannel} });
		return true;
	}

	return false;
}

bool MainGraph::connectTrackMIDISend(
	TrackType type, int index, int slot, SendDstType dstType, int dstIndex) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	/** Get Dst Node ID */
	auto dstNode = this->getDstNodeIndex(dstType, dstIndex, true);
	if (dstNode == NodeIndex{}) { return false; }

	/** Add Link */
	if (this->addMIDISendLink(trackNode, slot, dstNode)) {
		this->addConnection({ {trackNode, midiChannelIndex}, {dstNode, midiChannelIndex} });
		return true;
	}

	return false;
}

bool MainGraph::connectTrackAudioSend(TrackType type, int index, int slot,
	SendDstType dstType, int dstIndex, int trackChannel, int dstChannel) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	/** Get Dst Node ID */
	auto dstNode = this->getDstNodeIndex(dstType, dstIndex, false);
	if (dstNode == NodeIndex{}) { return false; }

	/** Check Channel Num */
	if (auto track = this->getTrackProcessor(type, index)) {
		if (trackChannel >= track->getTotalNumOutputChannels()) { return false; }
	}
	else {
		return false;
	}

	if (dstType == SendDstType::ToDevice) {
		if (dstChannel >= this->getTotalNumOutputChannels()) { return false; }
	}
	else {
		if (auto track = this->getTrackProcessor(
			(dstType == SendDstType::ToAUX) ? TrackType::AuxTrack : TrackType::MasterTrack, dstIndex)) {
			if (trackChannel >= track->getTotalNumInputChannels()) { return false; }
		}
		else {
			return false;
		}
	}

	/** Add Link */
	if (this->addAudioSendLink(trackNode, slot, dstNode, trackChannel, dstChannel)) {
		this->addConnection({ {trackNode, trackChannel}, {dstNode, dstChannel} });
		return true;
	}

	return false;
}

bool MainGraph::disconnectTrackMIDIInput(TrackType type, int index) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	auto inputNode = this->midiInputNode->nodeID;

	/** Remove Link */
	if (this->removeMIDIInputLink(trackNode)) {
		this->removeConnection({ {inputNode, midiChannelIndex}, {trackNode, midiChannelIndex} });
		return true;
	}

	return false;
}

bool MainGraph::disconnectTrackAudioInput(
	TrackType type, int index, int inputChannel, int trackChannel) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	auto inputNode = this->audioInputNode->nodeID;

	/** Remove Link */
	if (this->removeAudioInputLink(trackNode, inputChannel, trackChannel)) {
		this->removeConnection({ {inputNode, inputChannel}, {trackNode, trackChannel} });
		return true;
	}

	return false;
}

bool MainGraph::disconnectTrackMIDISend(
	TrackType type, int index, int slot, SendDstType dstType, int dstIndex) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	/** Get Dst Node ID */
	auto dstNode = this->getDstNodeIndex(dstType, dstIndex, true);
	if (dstNode == NodeIndex{}) { return false; }

	/** Remove Link */
	if (this->removeMIDISendLink(trackNode, slot, dstNode)) {
		this->removeConnection({ {trackNode, midiChannelIndex}, {dstNode, midiChannelIndex} });
		return true;
	}

	return false;
}

bool MainGraph::disconnectTrackMIDISend(TrackType type, int index, int slot) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	/** Remove Link */
	auto dstNode = this->removeMIDISendLink(trackNode, slot);
	if (dstNode != NodeIndex{}) {
		this->removeConnection({ {trackNode, midiChannelIndex}, {dstNode, midiChannelIndex} });
		return true;
	}

	return false;
}

bool MainGraph::disconnectTrackAudioSend(TrackType type, int index, int slot,
	SendDstType dstType, int dstIndex, int trackChannel, int dstChannel) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	/** Get Dst Node ID */
	auto dstNode = this->getDstNodeIndex(dstType, dstIndex, false);
	if (dstNode == NodeIndex{}) { return false; }

	/** Remove Link */
	if (this->removeAudioSendLink(trackNode, slot, dstNode, trackChannel, dstChannel)) {
		this->removeConnection({ {trackNode, trackChannel}, {dstNode, dstChannel} });
		return true;
	}

	return false;
}

bool MainGraph::disconnectTrackAudioSend(TrackType type, int index, int slot,
	SendDstType dstType, int dstIndex) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	/** Get Dst Node ID */
	auto dstNode = this->getDstNodeIndex(dstType, dstIndex, false);
	if (dstNode == NodeIndex{}) { return false; }

	/** Remove Link */
	auto channels = this->removeAudioSendLink(trackNode, slot, dstNode);
	if (!channels.empty()) {
		for (auto& channel : channels) {
			this->removeConnection({ {trackNode, channel.first}, {dstNode, channel.second} });
		}
		return true;
	}

	return false;
}

bool MainGraph::disconnectTrackAudioSend(TrackType type, int index, int slot) {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	/** Remove Link */
	auto group = this->removeAudioSendLink(trackNode, slot);
	if (group.first != NodeIndex{}) {
		for (auto& channel : group.second) {
			this->removeConnection({ {trackNode, channel.first}, {group.first, channel.second} });
		}
		return true;
	}

	return false;
}

bool MainGraph::isTrackMIDIInputConnected(TrackType type, int index) const {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return false; }

	/** Check Link */
	return this->checkMIDIInputLink(trackNode);
}

bool MainGraph::isTrackAudioInputConnected(TrackType type, int index) const {
	/** Get Channels */
	auto channels = this->getTrackAudioInputChannels(type, index);
	
	/** Check Empty */
	return !channels.empty();
}

const MainGraph::AudioChannelLinkList
MainGraph::getTrackAudioInputChannels(TrackType type, int index) const {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return {}; }

	/** Get Links */
	return this->getAudioInputChannels(trackNode);
}

bool MainGraph::isTrackMIDISendConnected(TrackType type, int index, int slot) const {
	/** Get Dst */
	auto dst = this->getTrackMIDISendDst(type, index, slot);

	/** Check Dst Valid */
	return dst.second >= 0;
}

const MainGraph::SendDst
MainGraph::getTrackMIDISendDst(TrackType type, int index, int slot) const {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return {}; }

	/** Get Dst */
	auto dstNode = this->getMIDISendSlot(trackNode, slot);

	/** Get Index */
	return this->findDst(dstNode);
}

bool MainGraph::isTrackAudioSendConnected(TrackType type, int index, int slot) const {
	/** Get Channels */
	auto channels = this->getTrackAudioSendChannels(type, index, slot);

	/** Check Empty */
	return !channels.empty();
}

const MainGraph::SendDst
MainGraph::getTrackAudioSendDst(TrackType type, int index, int slot) const {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return {}; }

	/** Get Dst */
	auto dstGroup = this->getAudioSendSlot(trackNode, slot);

	/** Get Index */
	return this->findDst(dstGroup.first);
}

const MainGraph::AudioChannelLinkList
MainGraph::getTrackAudioSendChannels(TrackType type, int index, int slot) const {
	/** Get Node ID */
	auto trackNode = this->getTrackNodeIndex(type, index);
	if (trackNode == NodeIndex{}) { return {}; }

	/** Get Dst */
	auto dstGroup = this->getAudioSendSlot(trackNode, slot);

	/** Get Channels */
	return dstGroup.second;
}

void MainGraph::setAudioLayout(int inputChannelNum, int outputChannelNum) {
	/** Create Buses Layout */
	juce::AudioProcessorGraph::BusesLayout busLayout;
	busLayout.inputBuses.add(juce::AudioChannelSet::discreteChannels(inputChannelNum));
	busLayout.outputBuses.add(juce::AudioChannelSet::discreteChannels(outputChannelNum));

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

	/** Auto Remove Connections */
	this->removeIllegalAudioInputConnections();
	this->removeIllegalAudioOutputConnections();

	/** Set Layout of Recorder */
	this->getRecorder()->setBusesLayout(inputLayout);

	/** Ensure Output Link of Master Track */
	//this->ensureMasterTrackOutputLink();

	/** Set Level Size */
	{
		juce::ScopedWriteLock locker(audioLock::getLevelMeterLock());
		this->outputLevels.resize(outputChannelNum);
	}
}

void MainGraph::setMIDIMessageHook(
	const std::function<void(const juce::MidiMessage&, bool)> hook) {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	this->midiHook = hook;
}

void MainGraph::setMIDICCListener(const MIDICCListener& listener) {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	this->ccListener = listener;
}

void MainGraph::clearMIDICCListener() {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	this->ccListener = MIDICCListener{};
}

void MainGraph::closeAllNote() {
	/** For Each Track */
	int trackNum = this->getTrackNum(TrackType::Track);
	for (int i = 0; i < trackNum; i++) {
		if (auto track = this->getTrackProcessor(TrackType::Track, i)) {
			if (auto seq = track->getSequencer()) {
				seq->closeAllNote();
			}
		}
	}
}

void MainGraph::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** Play Head */
	if (auto position = dynamic_cast<PlayPosition*>(this->getPlayHead())) {
		position->setSampleRate(sampleRate);
	}

	/** Renderer */
	Renderer::getInstance()->updateSampleRateAndBufferSize(
		sampleRate, maximumExpectedSamplesPerBlock);

	/** Recorder */
	this->recorder->prepareToPlay(
		sampleRate, maximumExpectedSamplesPerBlock);

	/** Current Graph */
	this->juce::AudioProcessorGraph::prepareToPlay(
		sampleRate, maximumExpectedSamplesPerBlock);
}

void MainGraph::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessorGraph::setPlayHead(newPlayHead);

	/** Recorder */
	this->getRecorder()->setPlayHead(newPlayHead);

	/** Master Track */
	if (auto track = this->getTrackProcessor(TrackType::MasterTrack, 0)) {
		track->setPlayHead(newPlayHead);
	}

	/** Aux Track */
	int auxTrackNum = this->getTrackNum(TrackType::AuxTrack);
	for (int i = 0; i < auxTrackNum; i++) {
		if (auto track = this->getTrackProcessor(TrackType::AuxTrack, i)) {
			track->setPlayHead(newPlayHead);
		}
	}

	/** Track */
	int trackNum = this->getTrackNum(TrackType::Track);
	for (int i = 0; i < trackNum; i++) {
		if (auto track = this->getTrackProcessor(TrackType::Track, i)) {
			track->setPlayHead(newPlayHead);
		}
	}
}

double MainGraph::getTailLengthSeconds() const {
	double result = 0;

	int trackNum = this->getTrackNum(TrackType::Track);
	for (int i = 0; i < trackNum; i++) {
		if (auto track = this->getTrackProcessor(TrackType::Track, i)) {
			result = std::max(track->getTailLengthSeconds(), result);
		}
	}
	
	return this->totalLengthTemp = std::max(result, this->totalLengthTemp);
}

SourceRecordProcessor* MainGraph::getRecorder() const {
	return this->recorder.get();
}

void MainGraph::clearGraph() {
	/** Clear Temp */
	this->totalLengthTemp = 0;

	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Clear Master Track */
	this->initMasterTrack();

	/** Remove Aux Tracks */
	for (int i = this->auxTrackList.size() - 1; i >= 0; i--) {
		/** Get ID */
		auto nodeID = this->auxTrackList.getUnchecked(i)->nodeID;

		/** Remove All Links */
		this->disconnectTrackLinks(nodeID);
		this->destoryTrackLinkTemps(nodeID);

		/** Remove Node */
		this->removeNode(nodeID);

		/** Remove Item */
		this->auxTrackList.remove(i);
	}
	
	/** Remove Tracks */
	for (int i = this->trackList.size() - 1; i >= 0; i--) {
		/** Get ID */
		auto nodeID = this->trackList.getUnchecked(i)->nodeID;

		/** Remove All Links */
		this->disconnectTrackLinks(nodeID);
		this->destoryTrackLinkTemps(nodeID);

		/** Remove Node */
		this->removeNode(nodeID);

		/** Remove Item */
		this->trackList.remove(i);
	}

	/** Reset Solo Count */
	utils::resetSoloCount();

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, -1);
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, -1);
}

const juce::Array<float> MainGraph::getOutputLevels() const {
	juce::ScopedReadLock locker(audioLock::getLevelMeterLock());
	return this->outputLevels;
}

void MainGraph::updateARAContext() {
	/** For Each Track */
	int trackNum = this->getTrackNum(TrackType::Track);
	for (int i = 0; i < trackNum; i++) {
		if (auto track = this->getTrackProcessor(TrackType::Track, i)) {
			if (auto seq = track->getSequencer()) {
				/** Update ARA Context Data for Recording Track */
				if (seq->getRecording() != SeqSourceProcessor::RecordState::NotRecording) {
					seq->syncARAContext();
				}
			}
		}
	}
}

void MainGraph::writeRecordingDataToSource(
	double startTime, double currentTime, double sampleRate,
	const juce::MidiMessageSequence& midiData, const juce::AudioSampleBuffer& audioData) {
	/** For Each Track */
	int trackNum = this->getTrackNum(TrackType::Track);
	for (int i = 0; i < trackNum; i++) {
		if (auto track = this->getTrackProcessor(TrackType::Track, i)) {
			if (auto seq = track->getSequencer()) {
				/** Update Source Data for Recording Track */
				if (seq->getRecording() != SeqSourceProcessor::RecordState::NotRecording) {
					/** Get Audio Links */
					auto linkTemp = this->getTrackAudioInputChannels(TrackType::Track, i);
					SeqSourceProcessor::ChannelLinkList links;
					for (auto& link : linkTemp) {
						links.add({ link.first, link.second });
					}

					seq->writeRecordingDataToSource(
						startTime, currentTime, sampleRate, midiData, audioData, links);
				}
			}
		}
	}
}

bool MainGraph::parse(
	const google::protobuf::Message* data,
	const ParseConfig& config) {
	auto mes = dynamic_cast<const vsp4::MainGraph*>(data);
	if (!mes) { return false; }

	this->clearGraph();

	if (mes->has_mastertrack()) {
		auto& masterTrack = mes->mastertrack();
		this->initMasterTrack(utils::getChannelSet(static_cast<utils::TrackType>(masterTrack.bus())));
		if (auto trackNode = this->getTrackProcessor(TrackType::MasterTrack, 0)) {
			if (!trackNode->parse(&masterTrack, config)) { return false; }
		}

		if (masterTrack.midiinput()) {
			this->connectTrackMIDIInput(TrackType::MasterTrack, 0);
		}
		auto& audioInput = masterTrack.audioinput();
		for (auto& i : audioInput) {
			this->connectTrackAudioInput(
				TrackType::MasterTrack, 0, i.srcchannel(), i.dstchannel());
		}
		auto& midiSend = masterTrack.midisend();
		for (int i = 0; i < MainGraph::midiSendSlotNum && i < midiSend.size(); i++) {
			auto& slot = midiSend.at(i);
			if (slot.dst() >= 0
				&& slot.type() == vsp4::SendDstType::TO_DEVICE) {
				this->connectTrackMIDISend(
					TrackType::MasterTrack, 0, i, SendDstType::ToDevice, 0);
			}
		}
		auto& audioSend = masterTrack.audiosend();
		for (int i = 0; i < MainGraph::audioSendSlotNum && i < audioSend.size(); i++) {
			auto& slot = audioSend.at(i);
			if (slot.dst() >= 0
				&& slot.type() == vsp4::SendDstType::TO_DEVICE) {
				this->connectTrackAudioSend(
					TrackType::MasterTrack, 0, i, SendDstType::ToDevice, 0,
					slot.srcchannel(), slot.dstchannel());
			}
		}
	}
	
	auto& auxTracks = mes->auxtracks();
	for (auto& i : auxTracks) {
		int trackIndex = this->getTrackNum(TrackType::AuxTrack);
		this->insertTrack(TrackType::AuxTrack, trackIndex,
			utils::getChannelSet(static_cast<utils::TrackType>(i.bus())));
		if (auto trackNode = this->getTrackProcessor(TrackType::AuxTrack, trackIndex)) {
			if (!trackNode->parse(&i, config)) { return false; }
		}
	}
	for (int i = 0; i < auxTracks.size(); i++) {
		auto& auxTrack = auxTracks.at(i);

		if (auxTrack.midiinput()) {
			this->connectTrackMIDIInput(TrackType::AuxTrack, i);
		}
		auto& audioInput = auxTrack.audioinput();
		for (auto& j : audioInput) {
			this->connectTrackAudioInput(
				TrackType::AuxTrack, i, j.srcchannel(), j.dstchannel());
		}
		auto& midiSend = auxTrack.midisend();
		for (int j = 0; j < MainGraph::midiSendSlotNum && j < midiSend.size(); j++) {
			auto& slot = midiSend.at(j);
			if (slot.dst() >= 0) {
				this->connectTrackMIDISend(
					TrackType::AuxTrack, i, j,
					static_cast<SendDstType>(slot.type()), slot.dst());
			}
		}
		auto& audioSend = auxTrack.audiosend();
		for (int j = 0; j < MainGraph::audioSendSlotNum && j < audioSend.size(); j++) {
			auto& slot = audioSend.at(j);
			if (slot.dst() >= 0) {
				this->connectTrackAudioSend(
					TrackType::AuxTrack, i, j,
					static_cast<SendDstType>(slot.type()), slot.dst(),
					slot.srcchannel(), slot.dstchannel());
			}
		}
	}

	auto& tracks = mes->tracks();
	for (auto& i : tracks) {
		int trackIndex = this->getTrackNum(TrackType::Track);
		this->insertTrack(TrackType::Track, trackIndex,
			utils::getChannelSet(static_cast<utils::TrackType>(i.bus())));
		if (auto trackNode = this->getTrackProcessor(TrackType::Track, trackIndex)) {
			if (!trackNode->parse(&i, config)) { return false; }
		}
	}
	for (int i = 0; i < tracks.size(); i++) {
		auto& track = tracks.at(i);

		if (track.midiinput()) {
			this->connectTrackMIDIInput(TrackType::Track, i);
		}
		auto& audioInput = track.audioinput();
		for (auto& j : audioInput) {
			this->connectTrackAudioInput(
				TrackType::Track, i, j.srcchannel(), j.dstchannel());
		}
		auto& midiSend = track.midisend();
		for (int j = 0; j < MainGraph::midiSendSlotNum && j < midiSend.size(); j++) {
			auto& slot = midiSend.at(j);
			if (slot.dst() >= 0) {
				this->connectTrackMIDISend(
					TrackType::Track, i, j,
					static_cast<SendDstType>(slot.type()), slot.dst());
			}
		}
		auto& audioSend = track.audiosend();
		for (int j = 0; j < MainGraph::audioSendSlotNum && j < audioSend.size(); j++) {
			auto& slot = audioSend.at(j);
			if (slot.dst() >= 0) {
				this->connectTrackAudioSend(
					TrackType::Track, i, j,
					static_cast<SendDstType>(slot.type()), slot.dst(),
					slot.srcchannel(), slot.dstchannel());
			}
		}
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> MainGraph::serialize(
	const SerializeConfig& config) const {
	auto mes = std::make_unique<vsp4::MainGraph>();

	if (auto masterTrack = this->getTrackProcessor(TrackType::MasterTrack, 0)) {
		auto tmes = std::unique_ptr<vsp4::Track>{ 
			dynamic_cast<vsp4::Track*>(masterTrack->serialize(config).release()) };
		if (!tmes) { return nullptr; }

		tmes->set_midiinput(this->isTrackMIDIInputConnected(TrackType::MasterTrack, 0));
		auto audioInput = this->getTrackAudioInputChannels(TrackType::MasterTrack, 0);
		for (auto& i : audioInput) {
			auto link = tmes->add_audioinput();
			link->set_srcchannel(i.first);
			link->set_dstchannel(i.second);
		}
		for (int slot = 0; slot < MainGraph::midiSendSlotNum; slot++) {
			auto dst = this->getTrackMIDISendDst(TrackType::MasterTrack, 0, slot);

			auto connection = tmes->add_midisend();
			connection->set_type(static_cast<vsp4::SendDstType>(dst.first));
			connection->set_dst(dst.second);
		}
		for (int slot = 0; slot < MainGraph::audioSendSlotNum; slot++) {
			auto dst = this->getTrackAudioSendDst(TrackType::MasterTrack, 0, slot);
			auto channels = this->getTrackAudioSendChannels(TrackType::MasterTrack, 0, slot);

			for (auto& i : channels) {
				auto connection = tmes->add_audiosend();
				connection->set_type(static_cast<vsp4::SendDstType>(dst.first));
				connection->set_dst(dst.second);
				connection->set_srcchannel(i.first);
				connection->set_dstchannel(i.second);
			}
		}

		mes->set_allocated_mastertrack(tmes.release());
	}

	auto auxTracks = mes->mutable_auxtracks();
	int auxTrackNum = this->getTrackNum(TrackType::AuxTrack);
	for (int i = 0; i < auxTrackNum; i++) {
		if (auto auxTrack = this->getTrackProcessor(TrackType::AuxTrack, i)) {
			auto tmes = std::unique_ptr<vsp4::Track>{
				dynamic_cast<vsp4::Track*>(auxTrack->serialize(config).release()) };
			if (!tmes) { return nullptr; }

			tmes->set_midiinput(this->isTrackMIDIInputConnected(TrackType::AuxTrack, i));
			auto audioInput = this->getTrackAudioInputChannels(TrackType::AuxTrack, i);
			for (auto& i : audioInput) {
				auto link = tmes->add_audioinput();
				link->set_srcchannel(i.first);
				link->set_dstchannel(i.second);
			}
			for (int slot = 0; slot < MainGraph::midiSendSlotNum; slot++) {
				auto dst = this->getTrackMIDISendDst(TrackType::AuxTrack, i, slot);

				auto connection = tmes->add_midisend();
				connection->set_type(static_cast<vsp4::SendDstType>(dst.first));
				connection->set_dst(dst.second);
			}
			for (int slot = 0; slot < MainGraph::audioSendSlotNum; slot++) {
				auto dst = this->getTrackAudioSendDst(TrackType::AuxTrack, i, slot);
				auto channels = this->getTrackAudioSendChannels(TrackType::AuxTrack, i, slot);

				for (auto& i : channels) {
					auto connection = tmes->add_audiosend();
					connection->set_type(static_cast<vsp4::SendDstType>(dst.first));
					connection->set_dst(dst.second);
					connection->set_srcchannel(i.first);
					connection->set_dstchannel(i.second);
				}
			}

			auxTracks->AddAllocated(tmes.release());
		}
	}

	auto tracks = mes->mutable_tracks();
	int trackNum = this->getTrackNum(TrackType::Track);
	for (int i = 0; i < trackNum; i++) {
		if (auto track = this->getTrackProcessor(TrackType::Track, i)) {
			auto tmes = std::unique_ptr<vsp4::Track>{
				dynamic_cast<vsp4::Track*>(track->serialize(config).release()) };
			if (!tmes) { return nullptr; }

			tmes->set_midiinput(this->isTrackMIDIInputConnected(TrackType::Track, i));
			auto audioInput = this->getTrackAudioInputChannels(TrackType::Track, i);
			for (auto& i : audioInput) {
				auto link = tmes->add_audioinput();
				link->set_srcchannel(i.first);
				link->set_dstchannel(i.second);
			}
			for (int slot = 0; slot < MainGraph::midiSendSlotNum; slot++) {
				auto dst = this->getTrackMIDISendDst(TrackType::Track, i, slot);

				auto connection = tmes->add_midisend();
				connection->set_type(static_cast<vsp4::SendDstType>(dst.first));
				connection->set_dst(dst.second);
			}
			for (int slot = 0; slot < MainGraph::audioSendSlotNum; slot++) {
				auto dst = this->getTrackAudioSendDst(TrackType::Track, i, slot);
				auto channels = this->getTrackAudioSendChannels(TrackType::Track, i, slot);

				for (auto& i : channels) {
					auto connection = tmes->add_audiosend();
					connection->set_type(static_cast<vsp4::SendDstType>(dst.first));
					connection->set_dst(dst.second);
					connection->set_srcchannel(i.first);
					connection->set_dstchannel(i.second);
				}
			}

			tracks->AddAllocated(tmes.release());
		}
	}

	return mes;
}

MainGraph::NodeIndex MainGraph::getTrackNodeIndex(TrackType type, int index) const {
	/** Check Track Type */
	if (type == TrackType::MasterTrack) {
		return this->masterTrack
			? this->masterTrack->nodeID : NodeIndex{};
	}

	/** Get Track List */
	auto& trackList = (type == TrackType::Track)
		? this->trackList : this->auxTrackList;

	/** Check Index */
	if (index < 0 || index >= trackList.size()) {
		return NodeIndex{};
	}

	/** Return Processor */
	return trackList.getUnchecked(index)->nodeID;
}

MainGraph::NodeIndex MainGraph::getDstNodeIndex(
	SendDstType type, int index, bool isMIDI) const {
	/** To Master Track */
	if (type == SendDstType::ToMaster) {
		return this->masterTrack
			? this->masterTrack->nodeID : NodeIndex{};
	}

	/** To Device */
	if (type == SendDstType::ToDevice) {
		return isMIDI ? this->midiOutputNode->nodeID : this->audioOutputNode->nodeID;
	}

	/** Get Track List */
	auto& trackList = this->auxTrackList;

	/** Check Index */
	if (index < 0 || index >= trackList.size()) {
		return NodeIndex{};
	}

	/** Return Processor */
	return trackList.getUnchecked(index)->nodeID;
}

bool MainGraph::addMIDIInputLink(NodeIndex track) {
	if (this->checkMIDIInputLink(track)) { return false; }

	this->midiInputLinks.insert(track);
	return true;
}

bool MainGraph::addAudioInputLink(
	NodeIndex track, int inputChannel, int trackChannel) {
	auto& audioInputChannels = this->getAudioInputChannels(track);
	AudioChannelLink link = { inputChannel, trackChannel };
	if (audioInputChannels.contains(link)) {
		return false;
	}

	audioInputChannels.insert(link);
	return true;
}

bool MainGraph::addMIDISendLink(
	NodeIndex track, int slot, NodeIndex dst) {
	if (track == dst) { return false; }
	if (slot < 0 || slot >= MainGraph::midiSendSlotNum) { return false; }

	for (int i = 0; i < MainGraph::midiSendSlotNum; i++) {
		if (i != slot) {
			auto& group = this->getMIDISendSlot(track, i);
			if (group == dst) {
				return false;
			}
		}
	}

	auto& midiSendSlot = this->getMIDISendSlot(track, slot);
	if (midiSendSlot == dst) {
		return false;
	}

	if (midiSendSlot != NodeIndex{}) {
		this->midiSendSrcTemp[midiSendSlot].extract(track);
	}

	midiSendSlot = dst;
	this->midiSendSrcTemp[dst].insert(track);
	return true;
}

bool MainGraph::addAudioSendLink(
	NodeIndex track, int slot, NodeIndex dst, int trackChannel, int dstChannel) {
	if (track == dst) { return false; }
	if (slot < 0 || slot >= MainGraph::audioSendSlotNum) { return false; }

	for (int i = 0; i < MainGraph::audioSendSlotNum; i++) {
		if (i != slot) {
			auto& group = this->getAudioSendSlot(track, i);
			if (group.first == dst) {
				return false;
			}
		}
	}

	auto& audioSendSlot = this->getAudioSendSlot(track, slot);
	AudioChannelLink link = { trackChannel, dstChannel };
	if (audioSendSlot.first == dst) {
		if (audioSendSlot.second.contains(link)) {
			return false;
		}
	}
	else {
		int oldNum = audioSendSlot.second.size();
		for (int i = 0; i < oldNum; i++) {
			this->audioSendSrcTemp[audioSendSlot.first].extract(track);
		}
		audioSendSlot.second.clear();
	}

	audioSendSlot.first = dst;
	audioSendSlot.second.insert(link);
	this->audioSendSrcTemp[dst].insert(track);
	return true;
}

bool MainGraph::removeMIDIInputLink(NodeIndex track) {
	auto it = this->midiInputLinks.find(track);
	if (it == this->midiInputLinks.end()) {
		return false;
	}

	this->midiInputLinks.erase(it);
	return true;
}

bool MainGraph::removeAudioInputLink(
	NodeIndex track, int inputChannel, int trackChannel) {
	auto& audioChannels = this->getAudioInputChannels(track);
	if (audioChannels.empty()) {
		return false;
	}

	AudioChannelLink link = { inputChannel, trackChannel };
	auto it = audioChannels.find(link);
	if (it == audioChannels.end()) {
		return false;
	}

	audioChannels.erase(it);
	return true;
}

const MainGraph::AudioChannelLinkList
MainGraph::removeAudioInputLink(NodeIndex track) {
	auto& audioChannels = this->getAudioInputChannels(track);
	if (audioChannels.empty()) {
		return {};
	}

	auto list = audioChannels;
	audioChannels = AudioChannelLinkList{};
	return list;
}

bool MainGraph::removeMIDISendLink(
	NodeIndex track, int slot, NodeIndex dst) {
	if (track == dst) { return false; }
	if (slot < 0 || slot >= MainGraph::midiSendSlotNum) { return false; }
	auto& midiSendSlot = this->getMIDISendSlot(track, slot);
	if (midiSendSlot != dst) {
		return false;
	}

	this->midiSendSrcTemp[dst].extract(track);
	midiSendSlot = NodeIndex{};
	return true;
}

MainGraph::NodeIndex MainGraph::removeMIDISendLink(NodeIndex track, int slot) {
	if (slot < 0 || slot >= MainGraph::midiSendSlotNum) { return {}; }
	auto& midiSendSlot = this->getMIDISendSlot(track, slot);

	auto index = midiSendSlot;
	midiSendSlot = NodeIndex{};

	if (index != NodeIndex{}) {
		this->midiSendSrcTemp[index].extract(track);
	}

	return index;
}

bool MainGraph::removeAudioSendLink(
	NodeIndex track, int slot, NodeIndex dst, int trackChannel, int dstChannel) {
	if (track == dst) { return false; }
	if (slot < 0 || slot >= MainGraph::audioSendSlotNum) { return false; }
	auto& audioSendSlot = this->getAudioSendSlot(track, slot);
	AudioChannelLink link = { trackChannel, dstChannel };
	if (audioSendSlot.first != dst) {
		return false;
	}

	auto it = audioSendSlot.second.find(link);
	if (it == audioSendSlot.second.end()) {
		return false;
	}

	this->audioSendSrcTemp[dst].extract(track);
	audioSendSlot.second.erase(it);
	if (audioSendSlot.second.empty()) {
		audioSendSlot.first = NodeIndex{};
	}
	return true;
}

const MainGraph::AudioChannelLinkList
MainGraph::removeAudioSendLink(NodeIndex track, int slot, NodeIndex dst) {
	if (track == dst) { return {}; }
	if (slot < 0 || slot >= MainGraph::audioSendSlotNum) { return {}; }
	auto& audioSendSlot = this->getAudioSendSlot(track, slot);
	if (audioSendSlot.first != dst) {
		return {};
	}

	auto list = audioSendSlot.second;
	audioSendSlot = AudioSendDstGroup{};

	for (int i = 0; i < list.size(); i++) {
		this->audioSendSrcTemp[dst].extract(track);
	}

	return list;
}

const MainGraph::AudioSendDstGroup MainGraph::removeAudioSendLink(NodeIndex track, int slot) {
	if (slot < 0 || slot >= MainGraph::audioSendSlotNum) { return {}; }
	auto& audioSendSlot = this->getAudioSendSlot(track, slot);

	auto group = audioSendSlot;
	audioSendSlot = AudioSendDstGroup{};

	if (group.first != NodeIndex{}) {
		for (int i = 0; i < group.second.size(); i++) {
			this->audioSendSrcTemp[group.first].extract(track);
		}
	}
	
	return group;
}

bool MainGraph::checkMIDIInputLink(NodeIndex track) const {
	return this->midiInputLinks.contains(track);
}

bool MainGraph::checkAudioInputLink(
	NodeIndex track, int inputChannel, int trackChannel) const {
	auto audioChannels = this->getAudioInputChannels(track);
	if (audioChannels.empty()) {
		return false;
	}

	AudioChannelLink link = { inputChannel, trackChannel };
	return audioChannels.contains(link);
}

bool MainGraph::checkAudioInputLink(NodeIndex track) const {
	auto audioChannels = this->getAudioInputChannels(track);
	return !audioChannels.empty();
}

bool MainGraph::checkMIDISendLink(
	NodeIndex track, int slot, NodeIndex dst) const {
	if (track == dst) { return false; }
	if (slot < 0 || slot >= MainGraph::midiSendSlotNum) { return false; }
	auto midiSendSlot = this->getMIDISendSlot(track, slot);
	return midiSendSlot == dst;
}

bool MainGraph::checkAudioSendLink(
	NodeIndex track, int slot, NodeIndex dst, int trackChannel, int dstChannel) const {
	if (track == dst) { return false; }
	if (slot < 0 || slot >= MainGraph::audioSendSlotNum) { return false; }
	auto audioSendSlot = this->getAudioSendSlot(track, slot);

	AudioChannelLink link = { trackChannel, dstChannel };
	return audioSendSlot.first == dst
		&& audioSendSlot.second.contains(link);
}

bool MainGraph::checkAudioSendLink(NodeIndex track, int slot, NodeIndex dst) const {
	if (track == dst) { return false; }
	if (slot < 0 || slot >= MainGraph::audioSendSlotNum) { return false; }
	auto audioSendSlot = this->getAudioSendSlot(track, slot);

	return audioSendSlot.first == dst;
}

const MainGraph::AudioChannelLinkList
MainGraph::getAudioInputChannels(NodeIndex track) const {
	auto channelsIt = this->audioInputLinks.find(track);
	if (channelsIt == this->audioInputLinks.end()) {
		return {};
	}

	return channelsIt->second;
}

MainGraph::AudioChannelLinkList&
MainGraph::getAudioInputChannels(NodeIndex track) {
	return this->audioInputLinks[track];
}

const MainGraph::NodeIndex MainGraph::getMIDISendSlot(NodeIndex track, int slot) const {
	auto trackMIDISendListIt = this->midiSendLinks.find(track);
	if (trackMIDISendListIt != this->midiSendLinks.end()) {
		auto& trackMIDISendList = trackMIDISendListIt->second;
		if (slot >= 0 && slot < trackMIDISendList.size()) {
			return trackMIDISendList[slot];
		}
	}
	return {};
}

const MainGraph::AudioSendDstGroup MainGraph::getAudioSendSlot(NodeIndex track, int slot) const {
	auto trackAudioSendListIt = this->audioSendLinks.find(track);
	if (trackAudioSendListIt != this->audioSendLinks.end()) {
		auto& trackAudioSendList = trackAudioSendListIt->second;
		if (slot >= 0 && slot < trackAudioSendList.size()) {
			return trackAudioSendList[slot];
		}
	}
	return {};
}

MainGraph::NodeIndex& MainGraph::getMIDISendSlot(NodeIndex track, int slot) {
	auto& trackMIDISendList = this->midiSendLinks[track];
	return trackMIDISendList[slot];
}

MainGraph::AudioSendDstGroup& MainGraph::getAudioSendSlot(NodeIndex track, int slot) {
	auto& trackAudioSendList = this->audioSendLinks[track];
	return trackAudioSendList[slot];
}

const std::set<MainGraph::NodeIndex>
MainGraph::getMIDISendLinkSrc(NodeIndex track) const {
	auto it = this->midiSendSrcTemp.find(track);
	if (it == this->midiSendSrcTemp.end()) {
		return {};
	}

	auto& list = it->second;
	return std::set<NodeIndex>{ list.begin(), list.end() };
}

const std::set<MainGraph::NodeIndex>
MainGraph::getAudioSendLinkSrc(NodeIndex track) const {
	auto it = this->audioSendSrcTemp.find(track);
	if (it == this->audioSendSrcTemp.end()) {
		return {};
	}

	auto& list = it->second;
	return std::set<NodeIndex>{ list.begin(), list.end() };
}

void MainGraph::disconnectTrackMIDIInputLinks(NodeIndex track) {
	if (this->checkMIDIInputLink(track)) {
		this->removeMIDIInputLink(track);
		this->removeConnection(
			{ {this->midiInputNode->nodeID, midiChannelIndex}, {track, midiChannelIndex} });
	}
}

void MainGraph::disconnectTrackAudioInputLinks(NodeIndex track) {
	auto links = this->removeAudioInputLink(track);
	for (auto& i : links) {
		this->removeConnection(
			{ {this->audioInputNode->nodeID, i.first}, {track, i.second} });
	}
}

void MainGraph::disconnectTrackMIDISendLinks(NodeIndex track) {
	for (int slot = 0; slot < MainGraph::midiSendSlotNum; slot++) {
		auto dstIndex = this->removeMIDISendLink(track, slot);
		if (dstIndex != NodeIndex{}) {
			this->removeConnection(
				{ {track, midiChannelIndex}, {dstIndex, midiChannelIndex} });
		}
	}
}

void MainGraph::disconnectTrackAudioSendLinks(NodeIndex track) {
	for (int slot = 0; slot < MainGraph::audioSendSlotNum; slot++) {
		auto group = this->removeAudioSendLink(track, slot);
		if (group.first != NodeIndex{}) {
			for (auto& i : group.second) {
				this->removeConnection(
					{ {track, i.first}, {group.first, i.second} });
			}
		}
	}
}

void MainGraph::disconnectTrackMIDISendInLinks(NodeIndex track) {
	auto srcList = this->getMIDISendLinkSrc(track);
	for (auto& src : srcList) {
		for (int slot = 0; slot < MainGraph::midiSendSlotNum; slot++) {
			if (this->removeMIDISendLink(src, slot, track)) {
				this->removeConnection(
					{ {src, midiChannelIndex}, {track, midiChannelIndex} });
			}
		}
	}
}

void MainGraph::disconnectTrackAudioSendInLinks(NodeIndex track) {
	auto srcList = this->getAudioSendLinkSrc(track);
	for (auto& src : srcList) {
		for (int slot = 0; slot < MainGraph::audioSendSlotNum; slot++) {
			auto channels = this->removeAudioSendLink(src, slot, track);
			for (auto& i : channels) {
				this->removeConnection(
					{ {src, i.first}, {track, i.second} });
			}
		}
	}
}

void MainGraph::disconnectTrackLinks(NodeIndex track) {
	this->disconnectTrackMIDIInputLinks(track);
	this->disconnectTrackAudioInputLinks(track);
	this->disconnectTrackMIDISendLinks(track);
	this->disconnectTrackAudioSendLinks(track);
	this->disconnectTrackMIDISendInLinks(track);
	this->disconnectTrackAudioSendInLinks(track);
}

void MainGraph::destoryTrackLinkTemps(NodeIndex track) {
	this->audioInputLinks.erase(track);
	this->midiSendLinks.erase(track);
	this->audioSendLinks.erase(track);

	this->midiSendSrcTemp.erase(track);
	this->audioSendSrcTemp.erase(track);
}

void MainGraph::removeIllegalAudioInputConnections() {
	this->removeIllegalMasterTrackAudioInputConnections();
	for (int i = 0; i < this->auxTrackList.size(); i++) {
		this->removeIllegalAuxTrackAudioInputConnections(i);
	}
	for (int i = 0; i < this->trackList.size(); i++) {
		this->removeIllegalTrackAudioInputConnections(i);
	}
}

void MainGraph::removeIllegalAudioOutputConnections() {
	this->removeIllegalMasterTrackAudioOutputConnections();
	for (int i = 0; i < this->auxTrackList.size(); i++) {
		this->removeIllegalAuxTrackAudioOutputConnections(i);
	}
	for (int i = 0; i < this->trackList.size(); i++) {
		this->removeIllegalTrackAudioOutputConnections(i);
	}
}

void MainGraph::removeIllegalMasterTrackAudioInputConnections() {
	/** Get Node */
	if (this->masterTrack) {
		auto nodeIndex = this->masterTrack->nodeID;

		/** Remove Connections */
		this->removeIllegalNodeAudioInputConnections(nodeIndex);
	}
}

void MainGraph::removeIllegalAuxTrackAudioInputConnections(int index) {
	/** Get Node */
	auto nodeIndex = this->auxTrackList.getUnchecked(index)->nodeID;

	/** Remove Connections */
	this->removeIllegalNodeAudioInputConnections(nodeIndex);
}

void MainGraph::removeIllegalTrackAudioInputConnections(int index) {
	/** Get Node */
	auto nodeIndex = this->trackList.getUnchecked(index)->nodeID;

	/** Remove Connections */
	this->removeIllegalNodeAudioInputConnections(nodeIndex);
}

void MainGraph::removeIllegalMasterTrackAudioOutputConnections() {
	/** Get Node */
	if (this->masterTrack) {
		auto nodeIndex = this->masterTrack->nodeID;

		/** Remove Connections */
		this->removeIllegalNodeAudioOutputConnections(nodeIndex);
	}
}

void MainGraph::removeIllegalAuxTrackAudioOutputConnections(int index) {
	/** Get Node */
	auto nodeIndex = this->auxTrackList.getUnchecked(index)->nodeID;

	/** Remove Connections */
	this->removeIllegalNodeAudioOutputConnections(nodeIndex);
}

void MainGraph::removeIllegalTrackAudioOutputConnections(int index) {
	/** Get Node */
	auto nodeIndex = this->trackList.getUnchecked(index)->nodeID;

	/** Remove Connections */
	this->removeIllegalNodeAudioOutputConnections(nodeIndex);
}

void MainGraph::removeIllegalNodeAudioSendInputConnections(NodeIndex track, int inputChannelNum) {
	/** For Each Input */
	auto audioInputNodes = this->getAudioSendLinkSrc(track);
	for (auto& inputNode : audioInputNodes) {
		/** For Each Slot */
		for (int slot = 0; slot < MainGraph::audioSendSlotNum; slot++) {
			auto group = this->getAudioSendSlot(inputNode, slot);
			if (group.first == track) {
				/** For Each Link */
				for (auto& i : group.second) {
					if (i.second >= inputChannelNum) {
						if (this->removeAudioSendLink(inputNode, slot, track, i.first, i.second)) {
							this->removeConnection({ {inputNode, i.first}, {track, i.second} });
						}
					}
				}
			}
		}
	}
}

void MainGraph::removeIllegalNodeAudioInputConnections(NodeIndex track) {
	/** Get Input Connections */
	auto channels = this->getAudioInputChannels(track);

	/** Get Input Channel Num */
	int channelNum = this->getTotalNumInputChannels();

	/** Get Audio Input Node */
	auto inputNode = this->audioInputNode->nodeID;

	/** Find Illegal Links */
	for (auto& i : channels) {
		if (i.first >= channelNum) {
			/** Disconnect Illegal Link */
			if (this->removeAudioInputLink(track, i.first, i.second)) {
				this->removeConnection({ {inputNode, i.first}, {track, i.second} });
			}
		}
	}
}

void MainGraph::removeIllegalNodeAudioOutputConnections(NodeIndex track) {
	/** Get Output Channel Num */
	int channelNum = this->getTotalNumOutputChannels();

	/** Get Audio Output Node */
	auto outputNode = this->audioOutputNode->nodeID;

	/** Get Each Slot */
	for (int slot = 0; slot < MainGraph::audioSendSlotNum; slot++) {
		auto group = this->getAudioSendSlot(track, slot);

		/** Check Send To Device */
		if (group.first == outputNode) {
			/** Find Illegal Links */
			for (auto& i : group.second) {
				if (i.first >= channelNum) {
					/** Disconnect Illegal Link */
					if (this->removeAudioSendLink(track, slot, outputNode, i.first, i.second)) {
						this->removeConnection({ {track, i.first}, {outputNode, i.second} });
					}
				}
			}
		}
	}
}

const MainGraph::TrackIndex MainGraph::findTrack(NodeIndex track) const {
	/** Master */
	if (this->masterTrack) {
		if (this->masterTrack->nodeID == track) {
			return { TrackType::MasterTrack, 0 };
		}
	}

	/** Track */
	for (int i = 0; i < this->trackList.size(); i++) {
		if (this->trackList.getUnchecked(i)->nodeID == track) {
			return { TrackType::Track, i };
		}
	}

	/** Aux Track */
	for (int i = 0; i < this->auxTrackList.size(); i++) {
		if (this->auxTrackList.getUnchecked(i)->nodeID == track) {
			return { TrackType::AuxTrack, i };
		}
	}

	/** Invalid */
	return { TrackType::Track, -1 };
}

const MainGraph::SendDst MainGraph::findDst(NodeIndex dst) const {
	/** Master Track */
	if (this->masterTrack) {
		if (this->masterTrack->nodeID == dst) {
			return { SendDstType::ToMaster, 0 };
		}
	}

	/** Device */
	if (this->audioOutputNode->nodeID == dst
		|| this->midiOutputNode->nodeID == dst) {
		return { SendDstType::ToDevice, 0 };
	}

	/** Aux Track */
	for (int i = 0; i < this->auxTrackList.size(); i++) {
		if (this->auxTrackList.getUnchecked(i)->nodeID == dst) {
			return { SendDstType::ToAUX, i };
		}
	}

	/** Invalid */
	return { SendDstType::ToAUX, -1 };
}

void MainGraph::ensureMasterTrackOutputLink() {
	/** Get Node ID */
	auto nodeID = this->getTrackNodeIndex(TrackType::MasterTrack, 0);
	if (nodeID == NodeIndex{}) { return; }

	auto outputNodeID = this->audioOutputNode->nodeID;

	/** Remove Old Links */
	this->disconnectTrackMIDISendLinks(nodeID);
	this->disconnectTrackAudioSendLinks(nodeID);

	/** Get Output Channels */
	if (auto track = this->getTrackProcessor(TrackType::MasterTrack, 0)) {
		int channelNum = std::min(this->getTotalNumOutputChannels(), track->getTotalNumOutputChannels());

		/** Link Channels */
		for (int i = 0; i < channelNum; i++) {
			if (this->addAudioSendLink(nodeID, 0, outputNodeID, i, i)) {
				this->addConnection({ {nodeID, i}, {outputNodeID, i} });
			}
		}
	}
}

void MainGraph::initMasterTrack(const juce::AudioChannelSet& bus) {
	/** Master Track Input Temp */
	/** Track, Slot, Channels */
	using AudioSendInputTemp = std::tuple<TrackIndex, int, AudioChannelLinkList>;
	using AudioSendInputTempList = juce::Array<AudioSendInputTemp>;
	/** Track, Slot */
	using MIDISendInputTemp = std::tuple<TrackIndex, int>;
	using MIDISendInputTempList = juce::Array<MIDISendInputTemp>;

	AudioSendInputTempList audioInputTemp;
	MIDISendInputTempList midiInputTemp;

	if (this->masterTrack) {
		/** Node ID */
		auto nodeID = this->masterTrack->nodeID;

		/** Get Audio Input Links */
		auto audioInputNodes = this->getAudioSendLinkSrc(nodeID);
		for (auto& inputNode : audioInputNodes) {
			/** Get Input Track Index */
			auto index = this->findTrack(inputNode);
			if (index.second >= 0) {
				for (int slot = 0; slot < MainGraph::audioSendSlotNum; slot++) {
					auto& group = this->getAudioSendSlot(inputNode, slot);
					if (group.first == nodeID) {
						audioInputTemp.add({ index, slot, group.second });
					}
				}
			}
		}

		/** Get MIDI Input Links */
		auto midiInputNodes = this->getMIDISendLinkSrc(nodeID);
		for (auto& inputNode : midiInputNodes) {
			/** Get Input Track Index */
			auto index = this->findTrack(inputNode);
			if (index.second >= 0) {
				for (int slot = 0; slot < MainGraph::midiSendSlotNum; slot++) {
					auto dst = this->getMIDISendSlot(inputNode, slot);
					if (dst == nodeID) {
						midiInputTemp.add({ index, slot });
					}
				}
			}
		}

		/** Remove Links */
		this->disconnectTrackLinks(nodeID);
		this->destoryTrackLinkTemps(nodeID);

		/** Remove Old Master Track */
		this->removeNode(nodeID);
		this->masterTrack = nullptr;
	}
	
	/** Init Master Track */
	if (this->masterTrack = this->addNode(std::make_unique<Track>(TrackType::MasterTrack, bus))) {
		/** Prepare To Play */
		this->masterTrack->getProcessor()->setPlayHead(this->getPlayHead());
		this->masterTrack->getProcessor()->prepareToPlay(this->getSampleRate(), this->getBlockSize());

		/** Set Index */
		dynamic_cast<Track*>(this->masterTrack->getProcessor())->updateIndex(0);

		/** Link Input */
		auto nodeID = this->masterTrack->nodeID;
		for (auto& [index, slot, channels] : audioInputTemp) {
			auto inputNode = this->getTrackNodeIndex(index.first, index.second);
			for (auto& i : channels) {
				if (i.second < bus.size()) {
					if (this->addAudioSendLink(inputNode, slot, nodeID, i.first, i.second)) {
						this->addConnection({ {inputNode, i.first}, {nodeID, i.second} });
					}
				}
			}
		}
		for (auto& [index, slot] : midiInputTemp) {
			auto inputNode = this->getTrackNodeIndex(index.first, index.second);
			if (this->addMIDISendLink(inputNode, slot, nodeID)) {
				this->addConnection({ {inputNode, midiChannelIndex}, {nodeID, midiChannelIndex} });
			}
		}

		/** Ensure Output */
		this->ensureMasterTrackOutputLink();
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, -1);
}

void MainGraph::processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) {
	/** Lock */
	juce::ScopedWriteLock levelLocker(audioLock::getLevelMeterLock());
	juce::ScopedTryReadLock audioLocker(audioLock::getAudioLock());
	juce::ScopedTryWriteLock sourceLocker(audioLock::getSourceLock());
	juce::ScopedTryReadLock pluginLocker(audioLock::getPluginLock());
	juce::ScopedTryWriteLock positionLocker(audioLock::getPositionLock());
	juce::ScopedTryReadLock controlLocker(audioLock::getAudioControlLock());
	if (!(audioLocker.isLocked() && pluginLocker.isLocked() 
		&& sourceLocker.isLocked()  && positionLocker.isLocked() && controlLocker.isLocked())) {
		vMath::zeroAllAudioData(audio);
		midi.clear();
		return;
	}

	/** Render State */
	bool isRendering = Renderer::getInstance()->getRendering();

	/** Call MIDI Hook */
	if(!isRendering) {
		if (this->midiHook) {
			for (auto m : midi) {
				juce::MessageManager::callAsync(
					[mes = m.getMessage(), hook = this->midiHook] {
						hook(mes, true);
					});
			}
		}
	}

	/** Send MIDI CC */
	if (!isRendering) {
		/** Get Last CC Channel */
		int lastCCChannel = -1;
		for (auto m : midi) {
			/** Get Message */
			auto message = m.getMessage();
			if (!message.isController()) { continue; }

			/** Auto Link Param */
			lastCCChannel = message.getControllerNumber();
		}

		/** Send Auto Connect */
		if ((lastCCChannel > -1) && this->ccListener) {
			juce::MessageManager::callAsync(
				std::bind(this->ccListener, lastCCChannel));
		}
	}

	/** Transport MMC */
	if (!isRendering) {
		for (auto m : midi) {
			auto mes = m.getMessage();
			if (mes.isMidiMachineControlMessage()) {
				switch (mes.getMidiMachineControlCommand())
				{
				case juce::MidiMessage::MidiMachineControlCommand::mmc_play:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->play(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_pause:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->pause(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_stop:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->stop(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_rewind:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->rewind(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_recordStart:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->record(true); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_recordStop:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->record(false); });
					continue;
				default:
					break;
				}
			}
		}
	}

	/** Truncate Input */
	if (isRendering) {
		vMath::zeroAllAudioData(audio);
		midi.clear();
	}

	/** Process Audio Block */
	{
		this->recorder->processBlock(audio, midi);
		this->juce::AudioProcessorGraph::processBlock(audio, midi);
	}

	/** Truncate Output */
	if (isRendering) {
		vMath::zeroAllAudioData(audio);
		midi.clear();
	}

	/** Get Level */
	for (int i = 0; i < audio.getNumChannels() && i < this->outputLevels.size(); i++) {
		this->outputLevels.getReference(i) =
			audio.getRMSLevel(i, 0, audio.getNumSamples());
	}

	/** MIDI Output */
	if (!isRendering) {
		if (this->midiHook) {
			for (auto m : midi) {
				juce::MessageManager::callAsync(
					[mes = m.getMessage(), hook = this->midiHook] {
						hook(mes, false);
					});
			}
		}
	}

	/** Add Position */
	if (auto position = dynamic_cast<PlayPosition*>(this->getPlayHead())) {
		/** Current Time */
		int currentPos = position->getPosition()->getTimeInSamples().orFallback(0);
		int clipSize = audio.getNumSamples();

		/** Check Loop */
		if (position->getLooping()) {
			auto [loopStart, loopEnd] = position->getLoopingTimeSec();
			double sampleRate = position->getSampleRate();

			/** Overflow */
			int next = currentPos + clipSize;
			if (next < (loopStart * sampleRate) || next > (loopEnd * sampleRate)) {
				position->setPositionInSamples(loopStart* sampleRate);
				return;
			}
		}
		
		/** Next Clip */
		if (INT_MAX - clipSize > currentPos) {
			position->next(clipSize);
		}
		else {
			/** Time Overflow */
			position->setOverflow();
		}
	}
}
