#include "MainGraph.h"
#include "../misc/AudioLock.h"
#include "../uiCallback/UICallback.h"

void MainGraph::insertSource(int index, const juce::AudioChannelSet& type) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Add To The Graph */
	if (auto ptrNode = this->addNode(std::make_unique<SeqSourceProcessor>(type))) {
		/** Limit Index */
		if (index < 0 || index > this->audioSourceNodeList.size()) {
			index = this->audioSourceNodeList.size();
		}

		/** Add Node To The Source List */
		this->audioSourceNodeList.insert(index, ptrNode);

		/** Prepare To Play */
		ptrNode->getProcessor()->setPlayHead(this->getPlayHead());
		ptrNode->getProcessor()->prepareToPlay(this->getSampleRate(), this->getBlockSize());

		/** Set Index */
		dynamic_cast<SeqSourceProcessor*>(ptrNode->getProcessor())->updateIndex(index);
		for (int i = index + 1; i < this->audioSourceNodeList.size(); i++) {
			auto node = this->audioSourceNodeList[i];
			dynamic_cast<SeqSourceProcessor*>(node->getProcessor())->updateIndex(i);
		}
	}
	else {
		jassertfalse;
	}
}

void MainGraph::removeSource(int index) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Limit Index */
	if (index < 0 || index >= this->audioSourceNodeList.size()) { return; }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->audioSourceNodeList.removeAndReturn(index);

	/** Remove MIDI Send Connection */
	this->midiSrc2TrkConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Remove Audio Send Connection */
	this->audioSrc2TrkConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Remove Node From Graph */
	this->removeNode(ptrNode->nodeID);

	/** Set Index */
	for (int i = index; i < this->audioSourceNodeList.size(); i++) {
		auto node = this->audioSourceNodeList[i];
		dynamic_cast<SeqSourceProcessor*>(node->getProcessor())->updateIndex(i);
	}
}

int MainGraph::getSourceNum() const {
	return this->audioSourceNodeList.size();
}

SeqSourceProcessor* MainGraph::getSourceProcessor(int index) const {
	if (index < 0 || index >= this->audioSourceNodeList.size()) { return nullptr; }
	return dynamic_cast<SeqSourceProcessor*>(
		this->audioSourceNodeList.getUnchecked(index)->getProcessor());
}

void MainGraph::setSourceBypass(int index, bool bypass) {
	if (index < 0 || index >= this->audioSourceNodeList.size()) { return; }
	if (auto node = this->audioSourceNodeList.getUnchecked(index)) {
		node->setBypassed(bypass);
	}
}

bool MainGraph::getSourceBypass(int index) const {
	if (index < 0 || index >= this->audioSourceNodeList.size()) { return false; }
	if (auto node = this->audioSourceNodeList.getUnchecked(index)) {
		return node->isBypassed();
	}
	return false;
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
