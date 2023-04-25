#include "Sequencer.h"
#include "Utils.h"

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

		/** Prepare To Play */
		ptrNode->getProcessor()->prepareToPlay(this->getSampleRate(), this->getBlockSize());
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

void Sequencer::insertInstrument(std::unique_ptr<juce::AudioProcessor> processor, int index) {
	/** Add To The Graph */
	auto ptrNode = this->addNode(std::move(processor));
	if (ptrNode) {
		/** Limit Index */
		if (index < 0 || index > this->instrumentNodeList.size()) {
			index = this->instrumentNodeList.size();
		}

		/** Add Node To The Source List */
		this->instrumentNodeList.insert(index, ptrNode);

		/** Prepare To Play */
		ptrNode->getProcessor()->prepareToPlay(this->getSampleRate(), this->getBlockSize());
	}
	else {
		jassertfalse;
	}
}

void Sequencer::removeInstrument(int index) {
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

int Sequencer::getSourceNum() const {
	return this->audioSourceNodeList.size();
}

juce::AudioProcessor* Sequencer::getSourceProcessor(int index) const {
	if (index < 0 || index >= this->audioSourceNodeList.size()) { return nullptr; }
	return this->audioSourceNodeList.getUnchecked(index)->getProcessor();
}

int Sequencer::getInstrumentNum() const {
	return this->instrumentNodeList.size();
}

juce::AudioProcessor* Sequencer::getInstrumentProcessor(int index) const {
	if (index < 0 || index >= this->instrumentNodeList.size()) { return nullptr; }
	return this->instrumentNodeList.getUnchecked(index)->getProcessor();
}

void Sequencer::setMIDIInputConnection(int sourceIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Remove Current Connection */
	this->removeMIDIInputConnection(sourceIndex);

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Add Connection */
	juce::AudioProcessorGraph::Connection connection =
		{ {this->midiInputNode->nodeID, this->midiChannelIndex},
		{nodeID, this->midiChannelIndex} };
	this->addConnection(connection);
	this->midiInputConnectionList.add(connection);
}

void Sequencer::removeMIDIInputConnection(int sourceIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Remove Connection */
	this->midiInputConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}

void Sequencer::setAudioInputConnection(int sourceIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (srcChannel < 0 || srcChannel >= this->getTotalNumInputChannels()) { return; }

	/** Get Node ID */
	auto ptrNode = this->audioSourceNodeList.getUnchecked(sourceIndex);
	auto nodeID = ptrNode->nodeID;

	/** Get Channels */
	int nodeChannels = ptrNode->getProcessor()->getTotalNumInputChannels();
	if (dstChannel < 0 || dstChannel >= nodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->audioInputConnectionList.add(connection);
	}
}

void Sequencer::removeAudioInputConnection(int sourceIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioInputConnectionList.removeAllInstancesOf(connection);
}

void Sequencer::addMIDIInstrumentConnection(int sourceIndex, int instrIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }

	/** Remove Current Connection */
	this->removeMIDIInstrumentConnection(sourceIndex, instrIndex);

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto dstNodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Add Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, this->midiChannelIndex}, {dstNodeID, this->midiChannelIndex} };
	this->addConnection(connection);
	this->midiInstrumentConnectionList.add(connection);
}

void Sequencer::removeMIDIInstrumentConnection(int sourceIndex, int instrIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto dstNodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Remove Connection */
	this->midiInputConnectionList.removeIf(
		[this, nodeID, dstNodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID &&
				element.destination.nodeID == dstNodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}

void Sequencer::setMIDISendConnection(int sourceIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Remove Current Connection */
	this->removeMIDISendConnection(sourceIndex);

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Add Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, this->midiChannelIndex},
	{this->midiOutputNode->nodeID, this->midiChannelIndex} };
	this->addConnection(connection);
	this->midiSendConnectionList.add(connection);
}

void Sequencer::removeMIDISendConnection(int sourceIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Remove Connection */
	this->midiSendConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}

void Sequencer::setAudioSendConnection(int sourceIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (dstChannel < 0 || dstChannel >= this->getTotalNumOutputChannels()) { return; }

	/** Get Node ID */
	auto ptrNode = this->audioSourceNodeList.getUnchecked(sourceIndex);
	auto nodeID = ptrNode->nodeID;

	/** Get Channels */
	auto nodeChannels = ptrNode->getProcessor()->getTotalNumOutputChannels();
	if (srcChannel < 0 || srcChannel >= nodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->audioSendConnectionList.add(connection);
	}
}

void Sequencer::removeAudioSendConnection(int sourceIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioSendConnectionList.removeAllInstancesOf(connection);
}

void Sequencer::setAudioOutputConnection(int instrIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }
	if (dstChannel < 0 || dstChannel >= this->getTotalNumOutputChannels()) { return; }

	/** Get Node ID */
	auto ptrNode = this->instrumentNodeList.getUnchecked(instrIndex);
	auto nodeID = ptrNode->nodeID;

	/** Get Channels */
	auto nodeChannels = ptrNode->getProcessor()->getTotalNumOutputChannels();
	if (srcChannel < 0 || srcChannel >= nodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->audioSendConnectionList.add(connection);
	}
}

void Sequencer::removeAudioOutputConnection(int instrIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioOutputConnectionList.removeAllInstancesOf(connection);
}

bool Sequencer::isMIDIInputConnected(int sourceIndex) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->midiInputNode->nodeID, this->midiChannelIndex}, {nodeID, this->midiChannelIndex} };
	return this->midiInputConnectionList.contains(connection);
}

bool Sequencer::isAudioInputConnected(int sourceIndex, int srcChannel, int dstChannel) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	return this->audioInputConnectionList.contains(connection);
}

bool Sequencer::isMIDIInstrumentConnected(int sourceIndex, int instrIndex) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto dstNodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, this->midiChannelIndex}, {dstNodeID, this->midiChannelIndex} };
	return this->midiInputConnectionList.contains(connection);
}

bool Sequencer::isMIDISendConnected(int sourceIndex) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, this->midiChannelIndex}, {this->midiOutputNode->nodeID, this->midiChannelIndex} };
	return this->midiSendConnectionList.contains(connection);
}

bool Sequencer::isAudioSendConnected(int sourceIndex, int srcChannel, int dstChannel) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	return this->audioSendConnectionList.contains(connection);
}

bool Sequencer::isAudioOutputConnected(int instrIndex, int srcChannel, int dstChannel) const {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	return this->audioOutputConnectionList.contains(connection);
}

void Sequencer::setInputChannels(const juce::Array<juce::AudioChannelSet>& channels) {
	auto currentBusLayout = this->getBusesLayout();
	currentBusLayout.inputBuses = channels;
	this->setAudioLayout(currentBusLayout);
}

void Sequencer::addOutputBus(const juce::AudioChannelSet& type) {
	auto currentBusLayout = this->getBusesLayout();
	currentBusLayout.outputBuses.add(type);
	this->setAudioLayout(currentBusLayout);
}

void Sequencer::removeOutputBus() {
	auto currentBusLayout = this->getBusesLayout();
	if (currentBusLayout.outputBuses.size() > 0) {
		currentBusLayout.outputBuses.removeLast();
		this->setAudioLayout(currentBusLayout);
	}
}

void Sequencer::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** Sequencer */
	this->juce::AudioProcessorGraph::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);

	/** Audio Source */
	for (auto& i : this->audioSourceNodeList) {
		auto src = i->getProcessor();
		src->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
	}

	/** Instrument */
	for (auto& i : this->instrumentNodeList) {
		auto instr = i->getProcessor();
		instr->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
	}
}

void Sequencer::setAudioLayout(const juce::AudioProcessorGraph::BusesLayout& busLayout) {
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
	this->removeIllegalAudioSendConnections();
	this->removeIllegalAudioOutputConnections();
}

void Sequencer::removeIllegalAudioInputConnections() {
	this->audioInputConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.channelIndex >= this->getTotalNumInputChannels()) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}

void Sequencer::removeIllegalAudioSendConnections() {
	this->audioSendConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.channelIndex >= this->getTotalNumOutputChannels()) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}

void Sequencer::removeIllegalAudioOutputConnections() {
	this->audioOutputConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.channelIndex >= this->getTotalNumOutputChannels()) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}
