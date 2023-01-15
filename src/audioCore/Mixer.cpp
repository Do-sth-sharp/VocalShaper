#include "Mixer.h"
#include "Track.h"

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

	/** Get Main Bus */
	int mainBusChannels = this->getMainBusNumInputChannels();

	/** Add Master Track */
	auto ptrMasterTrackNode = this->insertTrackInternal(0);

	/** Link Master Track Output To Main Output Node */
	if (ptrMasterTrackNode) {
		/** Link Node To Master Track */
		for (int i = 0; i < mainBusChannels; i++) {
			this->addConnection(
				{ {ptrMasterTrackNode->nodeID, i},{this->audioOutputNode->nodeID, i} });
		}
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
				{ {ptrNode->nodeID, i},{this->trackNodeList.getFirst()->nodeID, i} });
		}
	}
}
void Mixer::removeTrack(int index) {
	if (index != 0) {
		this->removeTrackInternal(index);
	}
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
