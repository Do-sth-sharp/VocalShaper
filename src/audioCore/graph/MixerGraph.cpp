#include "MainGraph.h"

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
	this->audioInstr2TrkConnectionList.removeIf(
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
}

int MainGraph::getTrackNum() const {
	return this->trackNodeList.size();
}

Track* MainGraph::getTrackProcessor(int index) const {
	if (index < 0 || index >= this->trackNodeList.size()) { return nullptr; }
	return dynamic_cast<Track*>(this->trackNodeList.getUnchecked(index)->getProcessor());
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
}

void MainGraph::removeAudioI2TrkConnection(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioI2TrkConnectionList.removeAllInstancesOf(connection);
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
}

void MainGraph::removeAudioTrk2OConnection(int trackIndex, int srcChannel, int dstChannel) {
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	auto nodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, srcChannel}, {this->audioOutputNode->nodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioTrk2OConnectionList.removeAllInstancesOf(connection);
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

MainGraph::TrackConnectionList MainGraph::getTrackInputFromTrackConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return MainGraph::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	MainGraph::TrackConnectionList resultList;

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
		int compareElements(MainGraph::TrackConnection& first, MainGraph::TrackConnection& second) {
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

MainGraph::TrackConnectionList MainGraph::getTrackInputFromSrcConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return MainGraph::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	MainGraph::TrackConnectionList resultList;

	for (auto& i : this->audioSrc2TrkConnectionList) {
		if (i.destination.nodeID == currentID) {
			/** Add To Result */
			resultList.add(std::make_tuple(
				-1, i.source.channelIndex, index, i.destination.channelIndex));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(MainGraph::TrackConnection& first, MainGraph::TrackConnection& second) {
			if (std::get<3>(first) == std::get<3>(second)) {
				return std::get<1>(first) - std::get<1>(second);
			}
			return std::get<3>(first) - std::get<3>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

MainGraph::TrackConnectionList MainGraph::getTrackInputFromInstrConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return MainGraph::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	MainGraph::TrackConnectionList resultList;

	for (auto& i : this->audioInstr2TrkConnectionList) {
		if (i.destination.nodeID == currentID) {
			/** Add To Result */
			resultList.add(std::make_tuple(
				-1, i.source.channelIndex, index, i.destination.channelIndex));
		}
	}

	/** Sort Result */
	class SortComparator {
	public:
		int compareElements(MainGraph::TrackConnection& first, MainGraph::TrackConnection& second) {
			if (std::get<3>(first) == std::get<3>(second)) {
				return std::get<1>(first) - std::get<1>(second);
			}
			return std::get<3>(first) - std::get<3>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

MainGraph::TrackConnectionList MainGraph::getTrackInputFromDeviceConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return MainGraph::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	MainGraph::TrackConnectionList resultList;

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
		int compareElements(MainGraph::TrackConnection& first, MainGraph::TrackConnection& second) {
			if (std::get<3>(first) == std::get<3>(second)) {
				return std::get<1>(first) - std::get<1>(second);
			}
			return std::get<3>(first) - std::get<3>(second);
		}
	} comparator;
	resultList.sort(comparator, true);

	return resultList;
}

MainGraph::TrackConnectionList MainGraph::getTrackOutputToTrackConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return MainGraph::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	MainGraph::TrackConnectionList resultList;

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
		int compareElements(MainGraph::TrackConnection& first, MainGraph::TrackConnection& second) {
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

MainGraph::TrackConnectionList MainGraph::getTrackOutputToDeviceConnections(int index) {
	/** Check Index */
	if (index < 0 || index >= this->trackNodeList.size()) {
		return MainGraph::TrackConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->trackNodeList.getUnchecked(index)->nodeID;
	MainGraph::TrackConnectionList resultList;

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
		int compareElements(MainGraph::TrackConnection& first, MainGraph::TrackConnection& second) {
			if (std::get<1>(first) == std::get<1>(second)) {
				return std::get<3>(first) - std::get<3>(second);
			}
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
}

int MainGraph::findSource(const SeqSourceProcessor* ptr) const {
	for (int i = 0; i < this->audioSourceNodeList.size(); i++) {
		if (this->audioSourceNodeList.getUnchecked(i)->getProcessor() == ptr) {
			return i;
		}
	}
	return -1;
}

int MainGraph::findInstr(const PluginDecorator* ptr) const {
	for (int i = 0; i < this->instrumentNodeList.size(); i++) {
		if (this->instrumentNodeList.getUnchecked(i)->getProcessor() == ptr) {
			return i;
		}
	}
	return -1;
}

int MainGraph::findTrack(const Track* ptr) const {
	for (int i = 0; i < this->trackNodeList.size(); i++) {
		if (this->trackNodeList.getUnchecked(i)->getProcessor() == ptr) {
			return i;
		}
	}
	return -1;
}
