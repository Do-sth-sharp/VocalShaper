#include "Mixer.h"
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
}

void Mixer::insertTrack(int index, const juce::AudioChannelSet& type) {
	this->insertTrackInternal(index, type);
}

void Mixer::removeTrack(int index) {
	this->removeTrackInternal(index);
}

int Mixer::getTrackNum() const {
	return this->trackNodeList.size();
}

Track* Mixer::getTrackProcessor(int index) const {
	if (index < 0 || index >= this->trackNodeList.size()) { return nullptr; }
	return dynamic_cast<Track*>(this->trackNodeList.getUnchecked(index)->getProcessor());
}

void Mixer::setTrackAudioInputFromSequencer(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }
	if (srcChannel < 0 || srcChannel >= this->sequencerChannelNum) { return; }

	/** Get Node ID */
	auto ptrNode = this->trackNodeList.getUnchecked(trackIndex);
	auto nodeID = ptrNode->nodeID;

	/** Get Channels */
	auto nodeChannels = ptrNode->getProcessor()->getTotalNumInputChannels();
	if (dstChannel < 0 || dstChannel >= nodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->trackAudioInputFromSequencerConnectionList.add(connection);
	}
}

void Mixer::setTrackAudioInputFromDevice(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }
	if (srcChannel < 0 || srcChannel >= this->getTotalNumInputChannels() - this->sequencerChannelNum) { return; }

	/** Get Node ID */
	auto ptrNode = this->trackNodeList.getUnchecked(trackIndex);
	auto nodeID = ptrNode->nodeID;

	/** Get Channels */
	auto nodeChannels = ptrNode->getProcessor()->getTotalNumInputChannels();
	if (dstChannel < 0 || dstChannel >= nodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel + this->sequencerChannelNum}, {nodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->trackAudioInputFromDeviceConnectionList.add(connection);
	}
}

void Mixer::setTrackAudioOutput(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }
	if (dstChannel < 0 || dstChannel >= this->getTotalNumOutputChannels()) { return; }

	/** Get Node ID */
	auto ptrNode = this->trackNodeList.getUnchecked(trackIndex);
	auto nodeID = ptrNode->nodeID;

	/** Get Channels */
	auto nodeChannels = ptrNode->getProcessor()->getTotalNumOutputChannels();
	if (srcChannel < 0 || srcChannel >= nodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->trackAudioOutputConnectionList.add(connection);
	}
}

void Mixer::removeTrackAudioInputFromSequencer(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	this->removeConnection(connection);
	this->trackAudioInputFromSequencerConnectionList.removeAllInstancesOf(connection);
}

void Mixer::removeTrackAudioInputFromDevice(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel + this->sequencerChannelNum}, {nodeID, dstChannel} };
	this->removeConnection(connection);
	this->trackAudioInputFromDeviceConnectionList.removeAllInstancesOf(connection);
}

void Mixer::removeTrackAudioOutput(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	this->removeConnection(connection);
	this->trackAudioOutputConnectionList.removeAllInstancesOf(connection);
}

bool Mixer::isTrackAudioInputFromSequencer(int trackIndex, int srcChannel, int dstChannel) const {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	return this->trackAudioInputFromSequencerConnectionList.contains(connection);
}

bool Mixer::isTrackAudioInputFromDevice(int trackIndex, int srcChannel, int dstChannel) const {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel + this->sequencerChannelNum}, {nodeID, dstChannel} };
	return this->trackAudioInputFromDeviceConnectionList.contains(connection);
}

bool Mixer::isTrackAudioOutput(int trackIndex, int srcChannel, int dstChannel) const {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	return this->trackAudioOutputConnectionList.contains(connection);
}

void Mixer::setTrackSend(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) {
	/** Track Can't Send To Itself */
	if (trackIndex == dstTrackIndex) { jassertfalse; return; }

	/** Check Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }
	if (dstTrackIndex < 0 || dstTrackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto ptrNode = this->trackNodeList.getUnchecked(trackIndex);
	auto ptrDstNode = this->trackNodeList.getUnchecked(dstTrackIndex);
	auto nodeID = ptrNode->nodeID;
	auto dstNodeID = ptrDstNode->nodeID;

	/** Get Channels */
	int nodeChannels = ptrNode->getProcessor()->getTotalNumOutputChannels();
	int dstNodeChannels = ptrDstNode->getProcessor()->getTotalNumInputChannels();
	if (srcChannel < 0 || srcChannel >= nodeChannels) { return; }
	if (dstChannel < 0 || dstChannel >= dstNodeChannels) { return; }

	/** Link Node To Dst Track */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {dstNodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->trackAudioSendConnectionList.add(connection);
	}
}

void Mixer::removeTrackSend(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) {
	/** Track Can't Side Link To Itself */
	if (trackIndex == dstTrackIndex) { jassertfalse; return; }

	/** Check Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }
	if (dstTrackIndex < 0 || dstTrackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;
	auto dstNodeID = this->trackNodeList.getUnchecked(dstTrackIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {dstNodeID, dstChannel} };
	this->removeConnection(connection);
	this->trackAudioSendConnectionList.removeAllInstancesOf(connection);
}

bool Mixer::isTrackSend(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) const {
	/** Check Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }
	if (dstTrackIndex < 0 || dstTrackIndex >= this->trackNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;
	auto dstNodeID = this->trackNodeList.getUnchecked(dstTrackIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {dstNodeID, dstChannel} };
	return this->trackAudioSendConnectionList.contains(connection);
}

void Mixer::setOutputChannels(const juce::Array<juce::AudioChannelSet>& channels) {
	auto currentBusLayout = this->getBusesLayout();
	currentBusLayout.outputBuses = channels;
	this->setAudioLayout(currentBusLayout);
}

void Mixer::addSequencerBus(const juce::AudioChannelSet& type) {
	/** Increase Sequencer Bus Num */
	this->sequencerBusNum++;
	this->sequencerChannelNum += type.size();

	/** Add Sequencer Bus Before Audio Input Device Channel */
	auto currentBusLayout = this->getBusesLayout();
	currentBusLayout.inputBuses.insert(
		this->sequencerBusNum - 1, type);

	/** Copy Track Input Bus Connection From Device */
	juce::Array<juce::AudioProcessorGraph::Connection> connectionTemp =
		this->trackAudioInputFromDeviceConnectionList;

	/** Remove All Input Buses Connection From Device */
	for (auto& i : this->trackAudioInputFromDeviceConnectionList) {
		this->removeConnection(i);
	}
	this->trackAudioInputFromDeviceConnectionList.clear();

	/** Set Audio Bus Layout */
	this->setAudioLayout(currentBusLayout);

	/** Recovery Input Buses Connection From Device */
	for (auto& i : connectionTemp) {
		i.source.channelIndex += type.size();
		this->addConnection(i);
		this->trackAudioInputFromDeviceConnectionList.add(i);
	}
}

void Mixer::removeSequencerBus() {
	/** Remove Sequencer Bus */
	if (this->sequencerBusNum > 0) {
		/** Decrease Sequencer Bus Num */
		this->sequencerBusNum--;

		/** Remove Sequencer Bus Before Audio Input Device Channel */
		auto currentBusLayout = this->getBusesLayout();
		auto lastBusChannelNum = currentBusLayout.outputBuses.getUnchecked(this->sequencerBusNum).size();
		currentBusLayout.outputBuses.remove(this->sequencerBusNum);

		/** Decrease Sequencer Channel Num */
		this->sequencerChannelNum -= lastBusChannelNum;

		/** Copy Track Input Bus Connection From Device */
		juce::Array<juce::AudioProcessorGraph::Connection> connectionTemp =
			this->trackAudioInputFromDeviceConnectionList;

		/** Remove All Input Buses Connection From Device */
		for (auto& i : this->trackAudioInputFromDeviceConnectionList) {
			this->removeConnection(i);
		}
		this->trackAudioInputFromDeviceConnectionList.clear();

		/** Set Audio Bus Layout */
		this->setAudioLayout(currentBusLayout);

		/** Recovery Input Buses Connection From Device */
		for (auto& i : connectionTemp) {
			i.source.channelIndex -= lastBusChannelNum;
			this->addConnection(i);
			this->trackAudioInputFromDeviceConnectionList.add(i);
		}
	}
}

int Mixer::getSequencerBusNum() const {
	return this->sequencerBusNum;
}

void Mixer::setInputDeviceChannels(const juce::Array<juce::AudioChannelSet>& channels) {
	/** Get Current Bus Layout */
	auto currentBusLayout = this->getBusesLayout();

	/** Remove Device Channels */
	currentBusLayout.inputBuses.resize(this->sequencerBusNum);

	/** Add New Device Channels */
	currentBusLayout.inputBuses.addArray(channels);

	/** Set Audio Bus Layout */
	this->setAudioLayout(currentBusLayout);
}

Mixer::TrackConnectionList Mixer::getTrackInputFromTrackConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return Mixer::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID 
		= this->trackNodeList.getUnchecked(index)->nodeID;
	Mixer::TrackConnectionList resultList;

	for (auto& i : this->trackAudioSendConnectionList) {
		if (i.destination.nodeID == currentID) {
			/** Get Source Track Index */
			int sourceIndex = this->trackNodeList.indexOf(
				this->getNodeForId(i.source.nodeID));
			if (sourceIndex < 0 || sourceIndex >= this->trackNodeList.size()) {
				continue;
			}

			/** Add To Result */
			resultList.add(std::make_tuple(
				sourceIndex, i.source.channelIndex, index, i.destination.channelIndex));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(Mixer::TrackConnection& first, Mixer::TrackConnection& second) {
			if (std::get<3>(first) == std::get<3>(second)) {
				if (std::get<0>(first) == std::get<0>(second)) {
					return std::get<1>(first) - std::get<1>(second);
				}
				return std::get<0>(first) - std::get<0>(second);
			}
			return std::get<3>(first) - std::get<3>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

Mixer::TrackConnectionList Mixer::getTrackInputFromSequencerConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return Mixer::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	Mixer::TrackConnectionList resultList;

	for (auto& i : this->trackAudioInputFromSequencerConnectionList) {
		if (i.destination.nodeID == currentID) {
			/** Add To Result */
			resultList.add(std::make_tuple(
				-1, i.source.channelIndex, index, i.destination.channelIndex));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(Mixer::TrackConnection& first, Mixer::TrackConnection& second) {
			if (std::get<3>(first) == std::get<3>(second)) {
				return std::get<1>(first) - std::get<1>(second);
			}
			return std::get<3>(first) - std::get<3>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

Mixer::TrackConnectionList Mixer::getTrackInputFromDeviceConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return Mixer::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	Mixer::TrackConnectionList resultList;

	for (auto& i : this->trackAudioInputFromDeviceConnectionList) {
		if (i.destination.nodeID == currentID) {
			/** Add To Result */
			resultList.add(std::make_tuple(
				-1, i.source.channelIndex - this->sequencerChannelNum,
				index, i.destination.channelIndex));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(Mixer::TrackConnection& first, Mixer::TrackConnection& second) {
			if (std::get<3>(first) == std::get<3>(second)) {
				return std::get<1>(first) - std::get<1>(second);
			}
			return std::get<3>(first) - std::get<3>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

Mixer::TrackConnectionList Mixer::getTrackOutputToTrackConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return Mixer::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	Mixer::TrackConnectionList resultList;

	for (auto& i : this->trackAudioSendConnectionList) {
		if (i.source.nodeID == currentID) {
			/** Get Destination Track Index */
			int destIndex = this->trackNodeList.indexOf(
				this->getNodeForId(i.destination.nodeID));
			if (destIndex < 0 || destIndex >= this->trackNodeList.size()) {
				continue;
			}

			/** Add To Result */
			resultList.add(std::make_tuple(
				index, i.source.channelIndex, destIndex, i.destination.channelIndex));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(Mixer::TrackConnection& first, Mixer::TrackConnection& second) {
			if (std::get<1>(first) == std::get<1>(second)) {
				if (std::get<2>(first) == std::get<2>(second)) {
					return std::get<3>(first) - std::get<3>(second);
				}
				return std::get<2>(first) - std::get<2>(second);
			}
			return std::get<1>(first) - std::get<1>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

Mixer::TrackConnectionList Mixer::getTrackOutputToDeviceConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return Mixer::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	Mixer::TrackConnectionList resultList;

	for (auto& i : this->trackAudioOutputConnectionList) {
		if (i.source.nodeID == currentID) {
			/** Add To Result */
			resultList.add(std::make_tuple(
				index, i.source.channelIndex, -1, i.destination.channelIndex));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(Mixer::TrackConnection& first, Mixer::TrackConnection& second) {
			if (std::get<1>(first) == std::get<1>(second)) {
				return std::get<3>(first) - std::get<3>(second);
			}
			return std::get<1>(first) - std::get<1>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
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
	if (index < 0 || index >= this->trackNodeList.size()) { return juce::AudioProcessorGraph::Node::Ptr(); }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->trackNodeList.removeAndReturn(index);
	auto nodeID = ptrNode->nodeID;

	/** Remove Input And Output Connections */
	this->trackAudioInputFromSequencerConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
	this->trackAudioInputFromDeviceConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
	this->trackAudioOutputConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
	this->trackAudioSendConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID ||
				element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Remove Node From Graph */
	this->removeNode(nodeID);

	return ptrNode;
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
	this->trackAudioInputFromSequencerConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.channelIndex >= this->getTotalNumInputChannels()) {
				this->removeConnection(element);
				return true;
			}
	return false;
		});
	this->trackAudioInputFromDeviceConnectionList.removeIf(
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
