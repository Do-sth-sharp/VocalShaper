#include "MainGraph.h"
#include "../misc/AudioLock.h"
#include "../uiCallback/UICallback.h"

void MainGraph::setMIDII2SrcConnection(int sourceIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Remove Current Connection */
	this->removeMIDII2SrcConnection(sourceIndex);

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Add Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->midiInputNode->nodeID, this->midiChannelIndex},
	{nodeID, this->midiChannelIndex} };
	this->addConnection(connection);
	this->midiI2SrcConnectionList.add(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, sourceIndex);
}

void MainGraph::removeMIDII2SrcConnection(int sourceIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Remove Connection */
	this->midiI2SrcConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, sourceIndex);
}

void MainGraph::setAudioI2SrcConnection(int sourceIndex, int srcChannel, int dstChannel) {
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (srcChannel < 0 || srcChannel >= this->getTotalNumInputChannels()) { return; }

	/** Get Node ID */
	auto ptrNode = this->audioSourceNodeList.getUnchecked(sourceIndex);
	auto nodeID = ptrNode->nodeID;

	/** Get Channels */
	auto nodeChannels = ptrNode->getProcessor()->getTotalNumInputChannels();
	if (dstChannel < 0 || dstChannel >= nodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->audioI2SrcConnectionList.add(connection);
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, sourceIndex);
}

void MainGraph::removeAudioI2SrcConnection(int sourceIndex, int srcChannel, int dstChannel) {
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioI2SrcConnectionList.removeAllInstancesOf(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, sourceIndex);
}

bool MainGraph::isMIDII2SrcConnected(int sourceIndex) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->midiInputNode->nodeID, this->midiChannelIndex}, {nodeID, this->midiChannelIndex} };
	return this->midiI2SrcConnectionList.contains(connection);
}

bool MainGraph::isAudioI2SrcConnected(int sourceIndex, int srcChannel, int dstChannel) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	return this->audioI2SrcConnectionList.contains(connection);
}

utils::AudioConnectionList MainGraph::getSourceInputFromDeviceConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->audioSourceNodeList.size()) {
		return utils::AudioConnectionList{};
	}

	/** Get Current Source ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->audioSourceNodeList.getUnchecked(index)->nodeID;
	utils::AudioConnectionList resultList;

	for (auto& i : this->audioI2SrcConnectionList) {
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

utils::AudioConnectionList MainGraph::getSourceOutputToTrackConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->audioSourceNodeList.size()) {
		return utils::AudioConnectionList{};
	}

	/** Get Current Source ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->audioSourceNodeList.getUnchecked(index)->nodeID;
	utils::AudioConnectionList resultList;

	for (auto& i : this->audioSrc2TrkConnectionList) {
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

utils::MidiConnectionList MainGraph::getSourceMidiInputFromDeviceConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->audioSourceNodeList.size()) {
		return utils::MidiConnectionList{};
	}

	/** Get Current Source ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->audioSourceNodeList.getUnchecked(index)->nodeID;
	utils::MidiConnectionList resultList;

	for (auto& i : this->midiI2SrcConnectionList) {
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

utils::MidiConnectionList MainGraph::getSourceMidiOutputToTrackConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->audioSourceNodeList.size()) {
		return utils::MidiConnectionList{};
	}

	/** Get Current Source ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->audioSourceNodeList.getUnchecked(index)->nodeID;
	utils::MidiConnectionList resultList;

	for (auto& i : this->midiSrc2TrkConnectionList) {
		if (i.source.nodeID == currentID) {
			/** Get Destination Track Index */
			int destIndex = this->trackNodeList.indexOf(
				this->getNodeForId(i.destination.nodeID));
			if (destIndex < 0 || destIndex >= this->trackNodeList.size()) {
				continue;
			}

			/** Add To Result */
			resultList.add(std::make_tuple(index, destIndex));
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

void MainGraph::closeAllNote() {
	for (auto& i : this->audioSourceNodeList) {
		auto seqTrack = dynamic_cast<SeqSourceProcessor*>(i->getProcessor());
		if (seqTrack) {
			seqTrack->closeAllNote();
		}
	}
}

int MainGraph::findSource(const SeqSourceProcessor* ptr) const {
	for (int i = 0; i < this->audioSourceNodeList.size(); i++) {
		if (this->audioSourceNodeList.getUnchecked(i)->getProcessor() == ptr) {
			return i;
		}
	}
	return -1;
}

void MainGraph::removeIllegalAudioI2SrcConnections() {
	this->audioI2SrcConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.channelIndex >= this->getTotalNumInputChannels()) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, -1);
}
