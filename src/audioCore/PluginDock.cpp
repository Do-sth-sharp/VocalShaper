#include "PluginDock.h"

PluginDock::PluginDock(const juce::AudioChannelSet& type) 
	: audioChannels(type) {
	/** Set Channel Layout */
	this->setChannelLayoutOfBus(true, 0, type);
	this->setChannelLayoutOfBus(false, 0, type);

	/** The Main Audio IO Node Of The Dock */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI Input Node Of The Dock */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));

	/** Connect The Audio Input And Output Node */
	int mainBusChannels = this->getMainBusNumInputChannels();
	for (int i = 0; i < mainBusChannels; i++) {
		this->addConnection(
			{ {this->audioInputNode->nodeID, i}, {this->audioOutputNode->nodeID, i} });
	}
}

void PluginDock::insertPlugin(std::unique_ptr<juce::AudioProcessor> processor, int index) {
	/** Add To The Graph */
	auto ptrNode = this->addNode(std::move(processor));
	if (ptrNode) {
		/** Limit Index */
		if (index < 0 || index > this->pluginNodeList.size()) {
			index = this->pluginNodeList.size();
		}

		/** Connect Node To The Audio Bus */
		{
			/** Find Hot Spot Nodes */
			juce::AudioProcessorGraph::Node::Ptr lastNode, nextNode;
			if (index == 0) {
				lastNode = this->audioInputNode;
			}
			else {
				lastNode = this->pluginNodeList.getUnchecked(index - 1);
			}
			if (index == this->pluginNodeList.size()) {
				nextNode = this->audioOutputNode;
			}
			else {
				nextNode = this->pluginNodeList.getUnchecked(index);
			}

			/** Get Main Bus */
			int mainBusChannels = this->getMainBusNumInputChannels();

			/** Remove Connection Between Hot Spot Nodes */
			for (int i = 0; i < mainBusChannels; i++) {
				this->removeConnection(
					{ {lastNode->nodeID, i}, {nextNode->nodeID, i} });
			}

			/** Add Connection To Hot Spot Nodes */
			for (int i = 0; i < mainBusChannels; i++) {
				this->addConnection(
					{ {lastNode->nodeID, i}, {ptrNode->nodeID, i} });
				this->addConnection(
					{ {ptrNode->nodeID, i}, {nextNode->nodeID, i} });
			}
		}

		/** Connect Node To MIDI Input */
		this->addConnection(
			{ {this->midiInputNode->nodeID, this->midiChannelIndex}, {ptrNode->nodeID, this->midiChannelIndex} });

		/** Add Node To The Plugin List */
		this->pluginNodeList.insert(index, ptrNode);
	}
	else {
		jassertfalse;
	}
}

void PluginDock::removePlugin(int index) {
	/** Limit Index */
	if (index < 0 || index >= this->pluginNodeList.size()) { return; }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->pluginNodeList.removeAndReturn(index);

	/** Remove Node From Graph */
	this->removeNode(ptrNode->nodeID);

	/** Connect The Last Node With The Next Node */
	{
		/** Find Hot Spot Nodes */
		juce::AudioProcessorGraph::Node::Ptr lastNode, nextNode;
		if (index == 0) {
			lastNode = this->audioInputNode;
		}
		else {
			lastNode = this->pluginNodeList.getUnchecked(index - 1);
		}
		if (index == this->pluginNodeList.size()) {
			nextNode = this->audioOutputNode;
		}
		else {
			nextNode = this->pluginNodeList.getUnchecked(index);
		}

		/** Get Main Bus */
		int mainBusChannels = this->getMainBusNumInputChannels();

		/** Add Connection Between Hot Spot Nodes */
		for (int i = 0; i < mainBusChannels; i++) {
			this->addConnection(
				{ {lastNode->nodeID, i}, {nextNode->nodeID, i} });
		}
	}
}
