#include "Mixer.h"
#include "Track.h"
#include "Utils.h"

Mixer::Mixer() {
	/** Set Channel Layout */
	this->setChannelLayoutOfBus(true, 0, juce::AudioChannelSet::stereo());
	this->setChannelLayoutOfBus(false, 0, juce::AudioChannelSet::stereo());

	/** The Main Audio IO Node Of The Mixer */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI Input Node Of The Mixer */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));

	/** Add Master Track */
	auto ptrMasterTrackNode = this->insertTrackInternal(0);

	/** Link Master Track Output To Main Output Node */
	if (ptrMasterTrackNode) {
		this->setTrackAudioOutput(0, 0);
	}
}

void Mixer::insertTrack(int index) {
	/** Limit Index */
	if (index == 0) { index = -1; }

	/** Add Node */
	auto ptrNode = this->insertTrackInternal(index, juce::AudioChannelSet::stereo());
	if (ptrNode) {
		/** Link Node To Master Track */
		this->setTrackSend(index, 0);
	}
}
void Mixer::removeTrack(int index) {
	if (index != 0) {
		this->removeTrackInternal(index);
	}
}

void Mixer::setTrackAudioInput(int trackIndex, int busIndex) {
	if (trackIndex < 0 || trackIndex > this->trackNodeList.size()) { return; }

	/** Remove Track Connections */
	this->removeTrackAudioInput(trackIndex);

	/** Get Bus */
	auto busChannels = utils::getChannelIndexAndNumOfBus(this, busIndex, true);

	/** Link Bus */
	for (int i = 0; i < std::get<1>(busChannels); i++) {
		juce::AudioProcessorGraph::Connection connection =
		{ {this->audioInputNode->nodeID, std::get<0>(busChannels) + i},
		 {this->trackNodeList.getUnchecked(trackIndex)->nodeID, i} };
		this->addConnection(connection);
		this->trackAudioInputConnectionList.add(connection);
	}
}

void Mixer::setTrackAudioOutput(int trackIndex, int busIndex) {
	if (trackIndex < 0 || trackIndex > this->trackNodeList.size()) { return; }

	/** Remove Track Connections */
	this->removeTrackAudioOutput(trackIndex);

	/** Get Bus */
	auto busChannels = utils::getChannelIndexAndNumOfBus(this, busIndex, false);

	/** Link Bus */
	for (int i = 0; i < std::get<1>(busChannels); i++) {
		juce::AudioProcessorGraph::Connection connection =
		{ {this->trackNodeList.getUnchecked(trackIndex)->nodeID, i},
		{this->audioOutputNode->nodeID, std::get<0>(busChannels) + i} };
		this->addConnection(connection);
		this->trackAudioOutputConnectionList.add(connection);
	}
}

void Mixer::removeTrackAudioInput(int trackIndex) {
	if (trackIndex < 0 || trackIndex > this->trackNodeList.size()) { return; }

	auto trackNodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;
	auto inputNodeID = this->audioInputNode->nodeID;

	this->trackAudioInputConnectionList.removeIf(
		[this, trackNodeID, inputNodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == inputNodeID && element.destination.nodeID == trackNodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}

void Mixer::removeTrackAudioOutput(int trackIndex) {
	if (trackIndex < 0 || trackIndex > this->trackNodeList.size()) { return; }

	auto trackNodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;
	auto outputNodeID = this->audioOutputNode->nodeID;

	this->trackAudioInputConnectionList.removeIf(
		[this, trackNodeID, outputNodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == trackNodeID && element.destination.nodeID == outputNodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}

void Mixer::setAudioLayout(const juce::AudioProcessorGraph::BusesLayout& busLayout) {
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
	this->removeIllegalInputConnections();
	this->removeIllegalOutputConnections();
}

void Mixer::removeIllegalInputConnections() {
	this->trackAudioInputConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.channelIndex >= this->getTotalNumInputChannels()) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});
}

void Mixer::removeIllegalOutputConnections() {
	this->trackAudioOutputConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.channelIndex >= this->getTotalNumOutputChannels()) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});
}

void Mixer::setTrackSend(int trackIndex, int dstTrackIndex) {
	/** Track Can't Send To Itself */
	if (trackIndex == dstTrackIndex) { jassertfalse; return; }

	/** Check Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }
	if (dstTrackIndex < 0 || dstTrackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;
	auto dstNodeID = this->trackNodeList.getUnchecked(dstTrackIndex)->nodeID;

	/** Remove Current Send */
	this->trackAudioSendConnectionList.removeIf(
		[this, nodeID]
		(const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});

	/** Get Main Bus */
	int mainBusChannels = this->getMainBusNumInputChannels();

	/** Link Node To Dst Track */
	for (int i = 0; i < mainBusChannels; i++) {
		juce::AudioProcessorGraph::Connection connection =
		{ {nodeID, i}, {dstNodeID, i} };
		this->addConnection(connection);
		this->trackAudioSendConnectionList.add(connection);
	}
}

void Mixer::addTrackSideLink(int trackIndex, int dstTrackIndex, int dstBus) {
	/** Track Can't Side Link To Itself */
	if (trackIndex == dstTrackIndex) { jassertfalse; return; }

	/** Check Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }
	if (dstTrackIndex < 0 || dstTrackIndex >= this->trackNodeList.size()) { return; }

	/** Check Bus Index */
	if (dstBus <= 0) { jassertfalse; return; }

	/** Remove Current Side Link */
	this->removeTrackSideLink(trackIndex, dstTrackIndex);

	/** Get Node */
	auto node = this->trackNodeList.getUnchecked(trackIndex);
	auto dstNode = this->trackNodeList.getUnchecked(dstTrackIndex);

	/** Add Bus While Dst Bus Isn't Exists */
	while (dstNode->getProcessor()->getBusCount(true) <= dstBus) {
		auto ptrTrack = dynamic_cast<Track*>(dstNode->getProcessor());
		if (ptrTrack) {
			ptrTrack->addAdditionalAudioBus();
		}
		else {
			jassertfalse;
			return;
		}
	}

	/** Get Main Bus */
	int mainBusChannels = this->getMainBusNumInputChannels();

	/** Get Dst Bus */
	auto dstBusChannels
		= utils::getChannelIndexAndNumOfBus(dstNode->getProcessor(), dstBus, true);

	/** Link Node To Dst Track */
	for (int i = 0; i < mainBusChannels; i++) {
		juce::AudioProcessorGraph::Connection connection =
		{ {node->nodeID, i}, {dstNode->nodeID, std::get<0>(dstBusChannels) + i}};
		this->addConnection(connection);
		this->trackAudioSendConnectionList.add(connection);
	}
}

void Mixer::removeTrackSideLink(int trackIndex, int dstTrackIndex) {
	/** Track Can't Side Link To Itself */
	if (trackIndex == dstTrackIndex) { jassertfalse; return; }

	/** Check Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }
	if (dstTrackIndex < 0 || dstTrackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;
	auto dstNodeID = this->trackNodeList.getUnchecked(dstTrackIndex)->nodeID;

	/** Remove Connection */
	this->trackAudioOutputConnectionList.removeIf(
		[this, nodeID, dstNodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID &&
				element.destination.nodeID == dstNodeID) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});
}

juce::AudioProcessorGraph::Node::Ptr Mixer::insertTrackInternal(int index, const juce::AudioChannelSet& type) {
	/** Add Node To Graph */
	auto ptrNode = this->addNode(std::make_unique<Track>(type));
	if (ptrNode) {
		/** Limit Index */
		if (index < 0 || index > this->trackNodeList.size()) {
			index = this->trackNodeList.size();
		}

		/** Add Node To List */
		this->trackNodeList.insert(index, ptrNode);

		/** Connect Track Node To MIDI Input */
		this->addConnection(
			{ {this->midiInputNode->nodeID, this->midiChannelIndex},
			{ptrNode->nodeID, this->midiChannelIndex} });
	}
	else {
		jassertfalse;
	}

	return ptrNode;
}

juce::AudioProcessorGraph::Node::Ptr Mixer::removeTrackInternal(int index) {
	/** Limit Index */
	if (index < 0 || index > this->trackNodeList.size()) { return juce::AudioProcessorGraph::Node::Ptr(); }

	/** Remove Input And Output Connections */
	this->removeTrackAudioInput(index);
	this->removeTrackAudioOutput(index);

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->trackNodeList.removeAndReturn(index);

	/** Remove Node From Graph */
	this->removeNode(ptrNode->nodeID);

	return ptrNode;
}
