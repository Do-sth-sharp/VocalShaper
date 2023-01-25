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

void Sequencer::setAudioInputConnection(int sourceIndex, int busIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (busIndex < 0 || busIndex >= this->getBusCount(true)) { return; }

	/** Remove Current Connection */
	this->removeAudioInputConnection(sourceIndex);

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Get Bus */
	auto busChannels = utils::getChannelIndexAndNumOfBus(this, busIndex, true);

	/** Link Bus */
	for (int i = 0; i < std::get<1>(busChannels); i++) {
		juce::AudioProcessorGraph::Connection connection =
		{ {this->audioInputNode->nodeID, std::get<0>(busChannels) + i},
		 {nodeID, i} };
		this->addConnection(connection);
		this->audioInputConnectionList.add(connection);
	}
}

void Sequencer::removeAudioInputConnection(int sourceIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Remove Connection */
	this->audioInputConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
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

void Sequencer::setAudioSendConnection(int sourceIndex, int busIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (busIndex < 0 || busIndex >= this->getBusCount(false)) { return; }

	/** Remove Current Connection */
	this->removeAudioSendConnection(sourceIndex);

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Get Bus */
	auto busChannels = utils::getChannelIndexAndNumOfBus(this, busIndex, false);

	/** Link Bus */
	for (int i = 0; i < std::get<1>(busChannels); i++) {
		juce::AudioProcessorGraph::Connection connection =
			{ {nodeID, i},
			{this->audioOutputNode->nodeID, std::get<0>(busChannels) + i} };
		this->addConnection(connection);
		this->audioSendConnectionList.add(connection);
	}
}

void Sequencer::removeAudioSendConnection(int sourceIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Remove Connection */
	this->audioSendConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}

void Sequencer::setAudioOutputConnection(int instrIndex, int busIndex) {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }
	if (busIndex < 0 || busIndex >= this->getBusCount(false)) { return; }

	/** Remove Current Connection */
	this->removeAudioOutputConnection(instrIndex);

	/** Get Node ID */
	auto nodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Get Bus */
	auto busChannels = utils::getChannelIndexAndNumOfBus(this, busIndex, false);

	/** Link Bus */
	for (int i = 0; i < std::get<1>(busChannels); i++) {
		juce::AudioProcessorGraph::Connection connection =
		{ {nodeID, i},
		{this->audioOutputNode->nodeID, std::get<0>(busChannels) + i} };
		this->addConnection(connection);
		this->audioOutputConnectionList.add(connection);
	}
}

void Sequencer::removeAudioOutputConnection(int instrIndex) {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Remove Connection */
	this->audioOutputConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
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
