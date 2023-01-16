#include "Mixer.h"
#include "Track.h"
#include "Utils.h"

Mixer::Mixer() {
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

void Mixer::insertTrack(int index, const juce::AudioChannelSet& type) {
	/** Limit Index */
	if (index == 0) { index = -1; }

	/** Add Node */
	auto ptrNode = this->insertTrackInternal(index, type);
	if (ptrNode) {
		/** Get Main Bus */
		int mainBusChannels = this->getMainBusNumInputChannels();

		/** Link Node To Master Track */
		for (int i = 0; i < mainBusChannels; i++) {
			this->addConnection(
				{ {ptrNode->nodeID, i}, {this->trackNodeList.getFirst()->nodeID, i} });
		}
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
	}
	else {
		jassertfalse;
	}

	return ptrNode;
}

juce::AudioProcessorGraph::Node::Ptr Mixer::removeTrackInternal(int index) {
	/** Limit Index */
	if (index < 0 || index > this->trackNodeList.size()) { return juce::AudioProcessorGraph::Node::Ptr(); }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->trackNodeList.removeAndReturn(index);

	/** Remove Node From Graph */
	this->removeNode(ptrNode->nodeID);

	return ptrNode;
}
