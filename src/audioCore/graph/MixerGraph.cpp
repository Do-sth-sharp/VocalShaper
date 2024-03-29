﻿#include "MainGraph.h"
#include "../uiCallback/UICallback.h"

void MainGraph::insertTrack(int index, const juce::AudioChannelSet& type) {
	/** Add Node To Graph */
	if (auto ptrNode = this->addNode(std::make_unique<Track>(type))) {
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

		/** Prepare To Play */
		ptrNode->getProcessor()->setPlayHead(this->getPlayHead());
		ptrNode->getProcessor()->prepareToPlay(this->getSampleRate(), this->getBlockSize());

		/** Set Index */
		dynamic_cast<Track*>(ptrNode->getProcessor())->updateIndex(index);
		for (int i = index + 1; i < this->trackNodeList.size(); i++) {
			auto node = this->trackNodeList[i];
			dynamic_cast<Track*>(node->getProcessor())->updateIndex(i);
		}

		/** Callback */
		UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, index);
	}
	else {
		jassertfalse;
	}
}

void MainGraph::removeTrack(int index) {
	/** Limit Index */
	if (index < 0 || index >= this->trackNodeList.size()) { return; }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->trackNodeList.removeAndReturn(index);
	auto nodeID = ptrNode->nodeID;

	/** Remove Input And Output Connections */
	this->midiI2TrkConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
	this->midiSrc2TrkConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
	this->midiTrk2OConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
	this->audioSrc2TrkConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
	this->audioI2TrkConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
	this->audioTrk2TrkConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID ||
				element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
	this->audioTrk2OConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Remove Node From Graph */
	this->removeNode(nodeID);

	/** Set Index */
	for (int i = index; i < this->trackNodeList.size(); i++) {
		auto node = this->trackNodeList[i];
		dynamic_cast<Track*>(node->getProcessor())->updateIndex(i);
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, index);
}

int MainGraph::getTrackNum() const {
	return this->trackNodeList.size();
}

Track* MainGraph::getTrackProcessor(int index) const {
	if (index < 0 || index >= this->trackNodeList.size()) { return nullptr; }
	return dynamic_cast<Track*>(this->trackNodeList.getUnchecked(index)->getProcessor());
}

void MainGraph::setTrackBypass(int index, bool bypass) {
	if (index < 0 || index >= this->trackNodeList.size()) { return; }
	if (auto node = this->trackNodeList.getUnchecked(index)) {
		node->setBypassed(bypass);

		/** Callback */
		UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, index);
	}
}

bool MainGraph::getTrackBypass(int index) const {
	if (index < 0 || index >= this->trackNodeList.size()) { return false; }
	if (auto node = this->trackNodeList.getUnchecked(index)) {
		return node->isBypassed();
	}
	return false;
}

void MainGraph::setMIDII2TrkConnection(int trackIndex) {
	/** Limit Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Remove Current Connection */
	this->removeMIDII2TrkConnection(trackIndex);

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Add Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->midiInputNode->nodeID, this->midiChannelIndex},
	{nodeID, this->midiChannelIndex} };
	this->addConnection(connection);
	this->midiI2TrkConnectionList.add(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::removeMIDII2TrkConnection(int trackIndex) {
	/** Limit Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Remove Connection */
	this->midiI2TrkConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::setAudioI2TrkConnection(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }
	if (srcChannel < 0 || srcChannel >= this->getTotalNumInputChannels()) { return; }

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
		this->audioI2TrkConnectionList.add(connection);
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::removeAudioI2TrkConnection(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioI2TrkConnectionList.removeAllInstancesOf(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::setAudioTrk2OConnection(int trackIndex, int srcChannel, int dstChannel) {
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
		this->audioTrk2OConnectionList.add(connection);
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::removeAudioTrk2OConnection(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioTrk2OConnectionList.removeAllInstancesOf(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::setAudioTrk2TrkConnection(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) {
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
		this->audioTrk2TrkConnectionList.add(connection);
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, dstTrackIndex);
}

void MainGraph::removeAudioTrk2TrkConnection(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) {
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
	this->audioTrk2TrkConnectionList.removeAllInstancesOf(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, dstTrackIndex);
}

void MainGraph::setMIDITrk2OConnection(int trackIndex) {
	/** Limit Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Remove Current Connection */
	this->removeMIDITrk2OConnection(trackIndex);

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Add Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, this->midiChannelIndex} ,
		{this->midiOutputNode->nodeID, this->midiChannelIndex} };
	this->addConnection(connection);
	this->midiTrk2OConnectionList.add(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::removeMIDITrk2OConnection(int trackIndex) {
	/** Limit Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Remove Connection */
	this->midiTrk2OConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

bool MainGraph::isMIDII2TrkConnected(int trackIndex) const {
	/** Limit Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->midiInputNode->nodeID, this->midiChannelIndex}, {nodeID, this->midiChannelIndex} };
	return this->midiI2TrkConnectionList.contains(connection);
}

bool MainGraph::isAudioI2TrkConnected(int trackIndex, int srcChannel, int dstChannel) const {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	return this->audioI2TrkConnectionList.contains(connection);
}

bool MainGraph::isAudioTrk2OConnected(int trackIndex, int srcChannel, int dstChannel) const {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	return this->audioTrk2OConnectionList.contains(connection);
}

bool MainGraph::isAudioTrk2TrkConnected(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) const {
	/** Check Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }
	if (dstTrackIndex < 0 || dstTrackIndex >= this->trackNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;
	auto dstNodeID = this->trackNodeList.getUnchecked(dstTrackIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {dstNodeID, dstChannel} };
	return this->audioTrk2TrkConnectionList.contains(connection);
}

bool MainGraph::isMIDITrk2OConnected(int trackIndex) const {
	/** Limit Index */
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, this->midiChannelIndex},{this->midiOutputNode->nodeID, this->midiChannelIndex} };
	return this->midiTrk2OConnectionList.contains(connection);
}

utils::AudioConnectionList MainGraph::getTrackInputFromTrackConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return utils::AudioConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	utils::AudioConnectionList resultList;

	for (auto& i : this->audioTrk2TrkConnectionList) {
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
		int compareElements(utils::AudioConnection& first, utils::AudioConnection& second) {
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

utils::AudioConnectionList MainGraph::getTrackInputFromSrcConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return utils::AudioConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	utils::AudioConnectionList resultList;

	for (auto& i : this->audioSrc2TrkConnectionList) {
		if (i.destination.nodeID == currentID) {
			/** Get Source Track Index */
			int sourceIndex = this->audioSourceNodeList.indexOf(
				this->getNodeForId(i.source.nodeID));
			if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) {
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
		int compareElements(utils::AudioConnection& first, utils::AudioConnection& second) {
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

utils::AudioConnectionList MainGraph::getTrackInputFromDeviceConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return utils::AudioConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	utils::AudioConnectionList resultList;

	for (auto& i : this->audioI2TrkConnectionList) {
		if (i.destination.nodeID == currentID) {
			/** Add To Result */
			resultList.add(std::make_tuple(
				-1, i.source.channelIndex, index, i.destination.channelIndex));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(utils::AudioConnection& first, utils::AudioConnection& second) {
			if (std::get<3>(first) == std::get<3>(second)) {
				return std::get<1>(first) - std::get<1>(second);
			}
			return std::get<3>(first) - std::get<3>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

utils::AudioConnectionList MainGraph::getTrackOutputToTrackConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return utils::AudioConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	utils::AudioConnectionList resultList;

	for (auto& i : this->audioTrk2TrkConnectionList) {
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
		int compareElements(utils::AudioConnection& first, utils::AudioConnection& second) {
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

utils::AudioConnectionList MainGraph::getTrackOutputToDeviceConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return utils::AudioConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	utils::AudioConnectionList resultList;

	for (auto& i : this->audioTrk2OConnectionList) {
		if (i.source.nodeID == currentID) {
			/** Add To Result */
			resultList.add(std::make_tuple(
				index, i.source.channelIndex, -1, i.destination.channelIndex));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(utils::AudioConnection& first, utils::AudioConnection& second) {
			if (std::get<1>(first) == std::get<1>(second)) {
				return std::get<3>(first) - std::get<3>(second);
			}
			return std::get<1>(first) - std::get<1>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

utils::MidiConnectionList MainGraph::getTrackMidiInputFromSrcConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return utils::MidiConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	utils::MidiConnectionList resultList;

	for (auto& i : this->midiSrc2TrkConnectionList) {
		if (i.destination.nodeID == currentID) {
			/** Get Source Track Index */
			int sourceIndex = this->audioSourceNodeList.indexOf(
				this->getNodeForId(i.source.nodeID));
			if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) {
				continue;
			}

			/** Add To Result */
			resultList.add(std::make_tuple(sourceIndex, index));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(utils::MidiConnection& first, utils::MidiConnection& second) {
			return std::get<0>(first) - std::get<0>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

utils::MidiConnectionList MainGraph::getTrackMidiInputFromDeviceConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return utils::MidiConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	utils::MidiConnectionList resultList;

	for (auto& i : this->midiI2TrkConnectionList) {
		if (i.destination.nodeID == currentID) {
			/** Add To Result */
			resultList.add(std::make_tuple(-1, index));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(utils::MidiConnection& first, utils::MidiConnection& second) {
			return std::get<0>(first) - std::get<0>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

utils::MidiConnectionList MainGraph::getTrackMidiOutputToDeviceConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return utils::MidiConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	utils::MidiConnectionList resultList;

	for (auto& i : this->midiTrk2OConnectionList) {
		if (i.source.nodeID == currentID) {
			/** Add To Result */
			resultList.add(std::make_tuple(index, -1));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(utils::MidiConnection& first, utils::MidiConnection& second) {
			return std::get<1>(first) - std::get<1>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

void MainGraph::removeIllegalAudioI2TrkConnections() {
	this->audioI2TrkConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.channelIndex >= this->getTotalNumInputChannels()) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, -1);
}

void MainGraph::removeIllegalAudioTrk2OConnections() {
	this->audioTrk2OConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.channelIndex >= this->getTotalNumOutputChannels()) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, -1);
}

int MainGraph::findTrack(const Track* ptr) const {
	for (int i = 0; i < this->trackNodeList.size(); i++) {
		if (this->trackNodeList.getUnchecked(i)->getProcessor() == ptr) {
			return i;
		}
	}
	return -1;
}
