#include "MainGraph.h"

#include "PlayPosition.h"

void MainGraph::insertSource(std::unique_ptr<juce::AudioProcessor> processor, int index) {
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
		ptrNode->getProcessor()->setPlayHead(PlayPosition::getInstance());
		ptrNode->getProcessor()->prepareToPlay(this->getSampleRate(), this->getBlockSize());
	}
	else {
		jassertfalse;
	}
}

void MainGraph::removeSource(int index) {
	/** Limit Index */
	if (index < 0 || index >= this->audioSourceNodeList.size()) { return; }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->audioSourceNodeList.removeAndReturn(index);

	/** Remove MIDI Input Connection */
	this->midiI2SrcConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Remove Audio Input Connection */
	this->audioI2SrcConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Remove MIDI Instrument Connection */
	this->midiSrc2InstrConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

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
}

bool MainGraph::insertInstrument(std::unique_ptr<juce::AudioPluginInstance> processor, int index) {
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
		ptrNode->getProcessor()->setPlayHead(PlayPosition::getInstance());
		ptrNode->getProcessor()->prepareToPlay(this->getSampleRate(), this->getBlockSize());

		return true;
	}
	else {
		jassertfalse;
		return false;
	}
}

void MainGraph::removeInstrument(int index) {
	/** Limit Index */
	if (index < 0 || index >= this->instrumentNodeList.size()) { return; }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->instrumentNodeList.removeAndReturn(index);

	/** Close The Editor */
	if (auto processor = ptrNode->getProcessor()) {
		if (auto editor = juce::Component::SafePointer(processor->getActiveEditor())) {
			juce::MessageManager::callAsync([editor] {if (editor) { delete editor; }});
		}
	}

	/** Remove MIDI Instrument Connection */
	this->midiSrc2InstrConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Remove Audio Output Connection */
	this->audioInstr2TrkConnectionList.removeIf(
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

int MainGraph::getSourceNum() const {
	return this->audioSourceNodeList.size();
}

juce::AudioProcessor* MainGraph::getSourceProcessor(int index) const {
	if (index < 0 || index >= this->audioSourceNodeList.size()) { return nullptr; }
	return this->audioSourceNodeList.getUnchecked(index)->getProcessor();
}

int MainGraph::getInstrumentNum() const {
	return this->instrumentNodeList.size();
}

juce::AudioPluginInstance* MainGraph::getInstrumentProcessor(int index) const {
	if (index < 0 || index >= this->instrumentNodeList.size()) { return nullptr; }
	return dynamic_cast<juce::AudioPluginInstance*>(
		this->instrumentNodeList.getUnchecked(index)->getProcessor());
}

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
}

void MainGraph::setAudioI2SrcConnection(int sourceIndex, int srcChannel, int dstChannel) {
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
		this->audioI2SrcConnectionList.add(connection);
	}
}

void MainGraph::removeAudioI2SrcConnection(int sourceIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	this->removeConnection(connection);
	this->audioI2SrcConnectionList.removeAllInstancesOf(connection);
}

void MainGraph::setMIDII2InstrConnection(int instrIndex) {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }

	/** Remove Current Connection */
	this->removeMIDII2InstrConnection(instrIndex);

	/** Get Node ID */
	auto nodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Add Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->midiInputNode->nodeID, this->midiChannelIndex}, {nodeID, this->midiChannelIndex} };
	this->addConnection(connection);
	this->midiI2InstrConnectionList.add(connection);
}

void MainGraph::removeMIDII2InstrConnection(int instrIndex) {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Remove Connection */
	this->midiI2InstrConnectionList.removeIf(
		[this, nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
}

void MainGraph::setMIDISrc2InstrConnection(int sourceIndex, int instrIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }

	/** Remove Current Connection */
	this->removeMIDISrc2InstrConnection(sourceIndex, instrIndex);

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto dstNodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Add Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, this->midiChannelIndex}, {dstNodeID, this->midiChannelIndex} };
	this->addConnection(connection);
	this->midiSrc2InstrConnectionList.add(connection);
}

void MainGraph::removeMIDISrc2InstrConnection(int sourceIndex, int instrIndex) {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return; }
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto dstNodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Remove Connection */
	this->midiSrc2InstrConnectionList.removeIf(
		[this, nodeID, dstNodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.nodeID == nodeID &&
				element.destination.nodeID == dstNodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});
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

bool MainGraph::isMIDII2InstrConnected(int instrIndex) const {
	/** Limit Index */
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->midiInputNode->nodeID, this->midiChannelIndex}, {nodeID, this->midiChannelIndex} };
	return this->midiI2InstrConnectionList.contains(connection);
}

bool MainGraph::isMIDISrc2InstrConnected(int sourceIndex, int instrIndex) const {
	/** Limit Index */
	if (sourceIndex < 0 || sourceIndex >= this->audioSourceNodeList.size()) { return false; }
	if (instrIndex < 0 || instrIndex >= this->instrumentNodeList.size()) { return false; }

	/** Get Node ID */
	auto nodeID = this->audioSourceNodeList.getUnchecked(sourceIndex)->nodeID;
	auto dstNodeID = this->instrumentNodeList.getUnchecked(instrIndex)->nodeID;

	/** Find Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {nodeID, this->midiChannelIndex}, {dstNodeID, this->midiChannelIndex} };
	return this->midiSrc2InstrConnectionList.contains(connection);
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
}
