#include "Sequencer.h"

Sequencer::Sequencer() {
	/** The Main MIDI IO Node Of The Sequencer */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));
	this->midiOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode));

	/** The Main Audio IO Node Of The Sequencer */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));
}

void Sequencer::insertSource(std::unique_ptr<juce::AudioProcessor> processor, int index) {
	/** Add To The Graph */
	auto ptrNode = this->addNode(std::move(processor));
	if (ptrNode) {
		/** Limit Index */
		if (index < 0 || index > this->audioSourceNodeList.size()) {
			index = this->audioSourceNodeList.size();
		}

		/** Add Node To The Source List */
		this->audioSourceNodeList.insert(index, ptrNode);
	}
	else {
		jassertfalse;
	}
}

void Sequencer::removeSource(int index) {
	/** Limit Index */
	if (index < 0 || index >= this->audioSourceNodeList.size()) { return; }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->audioSourceNodeList.removeAndReturn(index);

	/** Remove MIDI Input Connection */
	this->midiInputConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});

	/** Remove Audio Input Connection */
	this->audioInputConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});

	/** Remove MIDI Instrument Connection */
	this->midiInstrumentConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});

	/** Remove MIDI Send Connection */
	this->midiSendConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});

	/** Remove Audio Send Connection */
	this->audioSendConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});

	/** Remove Node From Graph */
	this->removeNode(ptrNode->nodeID);
}

void Sequencer::insertInstrment(std::unique_ptr<juce::AudioProcessor> processor, int index) {
	/** Add To The Graph */
	auto ptrNode = this->addNode(std::move(processor));
	if (ptrNode) {
		/** Limit Index */
		if (index < 0 || index > this->instrumentNodeList.size()) {
			index = this->instrumentNodeList.size();
		}

		/** Add Node To The Source List */
		this->instrumentNodeList.insert(index, ptrNode);
	}
	else {
		jassertfalse;
	}
}

void Sequencer::removeInstrment(int index) {
	/** Limit Index */
	if (index < 0 || index >= this->instrumentNodeList.size()) { return; }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->instrumentNodeList.removeAndReturn(index);

	/** Remove MIDI Instrument Connection */
	this->midiInstrumentConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});

	/** Remove Audio Output Connection */
	this->audioOutputConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
		return false;
		});

	/** Remove Node From Graph */
	this->removeNode(ptrNode->nodeID);
}
