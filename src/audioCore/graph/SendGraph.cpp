#include "MainGraph.h"
#include "../uiCallback/UICallback.h"

void MainGraph::setMIDISrc2TrkConnection(int sourceIndex, int trackIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Remove Current Connection */
	this->removeMIDISrc2TrkConnection(sourceIndex, trackIndex);

	/** Get Node ID */
	auto srcNodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto trkNodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Add Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {srcNodeID, this->midiChannelIndex},
	{trkNodeID, this->midiChannelIndex} };
	this->addConnection(connection);
	this->midiSrc2TrkConnectionList.add(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::removeMIDISrc2TrkConnection(int sourceIndex, int trackIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto srcNodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto trkNodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Remove Connection */
	this->midiSrc2TrkConnectionList.removeIf(
		[this, srcNodeID, trkNodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == srcNodeID &&
				element.destination.nodeID == trkNodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::setAudioSrc2TrkConnection(int sourceIndex, int trackIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto ptrSrcNode = this->audioSourceNodeList.getUnchecked(sourceIndex);
	auto srcNodeID = ptrSrcNode->nodeID;
	auto ptrTrkNode = this->trackNodeList.getUnchecked(trackIndex);
	auto trkNodeID = ptrTrkNode->nodeID;

	/** Get Channels */
	auto srcNodeChannels = ptrSrcNode->getProcessor()->getTotalNumOutputChannels();
	if (srcChannel < 0 || srcChannel >= srcNodeChannels) { return; }
	auto trkNodeChannels = ptrTrkNode->getProcessor()->getTotalNumInputChannels();
	if (dstChannel < 0 || dstChannel >= trkNodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {srcNodeID, srcChannel}, {trkNodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->audioSrc2TrkConnectionList.add(connection);
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::removeAudioSrc2TrkConnection(int sourceIndex, int trackIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto srcNodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto trkNodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {srcNodeID, srcChannel}, {trkNodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioSrc2TrkConnectionList.removeAllInstancesOf(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::setAudioInstr2TrkConnection(int instrIndex, int trackIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto ptrInsNode = this->instrumentNodeList.getUnchecked(instrIndex);
	auto insNodeID = ptrInsNode->nodeID;
	auto ptrTrkNode = this->trackNodeList.getUnchecked(trackIndex);
	auto trkNodeID = ptrTrkNode->nodeID;

	/** Get Channels */
	auto insNodeChannels = ptrInsNode->getProcessor()->getTotalNumOutputChannels();
	if (srcChannel < 0 || srcChannel >= insNodeChannels) { return; }
	auto trkNodeChannels = ptrTrkNode->getProcessor()->getTotalNumInputChannels();
	if (dstChannel < 0 || dstChannel >= trkNodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {insNodeID, srcChannel}, {trkNodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->audioInstr2TrkConnectionList.add(connection);
	}

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, instrIndex);
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

void MainGraph::removeAudioInstr2TrkConnection(int instrIndex, int trackIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return; }

	/** Get Node ID */
	auto insNodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;
	auto trkNodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {insNodeID, srcChannel}, {trkNodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioInstr2TrkConnectionList.removeAllInstancesOf(connection);

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, instrIndex);
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, trackIndex);
}

bool MainGraph::isMIDISrc2TrkConnected(int sourceIndex, int trackIndex) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	/** Get Node ID */
	auto srcNodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto trkNodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {srcNodeID, this->midiChannelIndex}, {trkNodeID, this->midiChannelIndex} };
	return this->midiSrc2TrkConnectionList.contains(connection);
}

bool MainGraph::isAudioSrc2TrkConnected(int sourceIndex, int trackIndex, int srcChannel, int dstChannel) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	/** Get Node ID */
	auto srcNodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto trkNodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {srcNodeID, srcChannel}, {trkNodeID, dstChannel} };
	return this->audioSrc2TrkConnectionList.contains(connection);
}

bool MainGraph::isAudioInstr2TrkConnected(int instrIndex, int trackIndex, int srcChannel, int dstChannel) const {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return false; }
	if (trackIndex < 0 || trackIndex >= this->trackNodeList.size()) { return false; }

	/** Get Node ID */
	auto insNodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;
	auto trkNodeID = this->trackNodeList.getUnchecked(trackIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {insNodeID, srcChannel}, {trkNodeID, dstChannel} };
	return this->audioInstr2TrkConnectionList.contains(connection);
}
