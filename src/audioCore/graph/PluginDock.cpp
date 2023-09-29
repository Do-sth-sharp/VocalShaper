#include "PluginDock.h"

PluginDock::PluginDock(const juce::AudioChannelSet& type) 
	: audioChannels(type) {
	/** Set Channel Layout */
	juce::AudioProcessorGraph::BusesLayout layout;
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(type.size()));
	layout.outputBuses.add(
		juce::AudioChannelSet::discreteChannels(type.size()));
	this->setBusesLayout(layout);

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

	/** Set Audio Input Node Channel Num */
	juce::AudioProcessorGraph::BusesLayout inputLayout;
	inputLayout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels()));
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** Connect The Audio Input And Output Node */
	int mainBusChannels = this->getMainBusNumInputChannels();
	for (int i = 0; i < mainBusChannels; i++) {
		this->addConnection(
			{ {this->audioInputNode->nodeID, i}, {this->audioOutputNode->nodeID, i} });
	}
}

PluginDock::~PluginDock() {
	for (auto& i : this->pluginNodeList) {
		if (auto processor = i->getProcessor()) {
			if (auto editor = processor->getActiveEditor()) {
				delete editor;
			}
		}
	}
}

bool PluginDock::insertPlugin(std::unique_ptr<juce::AudioPluginInstance> processor, int index) {
	/** Add To The Graph */
	auto ptrNode = this->addNode(std::make_unique<PluginDecorator>(std::move(processor)));
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

			/** Get Plugin Bus */
			int pluginInputChannels = ptrNode->getProcessor()->getMainBusNumInputChannels();
			int pluginOutputChannels = ptrNode->getProcessor()->getMainBusNumOutputChannels();

			/** Check Channels */
			if (pluginInputChannels < mainBusChannels || pluginOutputChannels < mainBusChannels) {
				this->removeNode(ptrNode->nodeID);
				return false;
			}

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

		/** Prepare To Play */
		ptrNode->getProcessor()->setPlayHead(this->getPlayHead());
		ptrNode->getProcessor()->prepareToPlay(this->getSampleRate(), this->getBlockSize());

		return true;
	}
	else {
		jassertfalse;
		return false;
	}
}

void PluginDock::removePlugin(int index) {
	/** Limit Index */
	if (index < 0 || index >= this->pluginNodeList.size()) { return; }

	/** Get The Node Ptr Then Remove From The List */
	auto ptrNode = this->pluginNodeList.removeAndReturn(index);

	/** Close The Editor */
	if (auto processor = ptrNode->getProcessor()) {
		if (auto editor = juce::Component::SafePointer(processor->getActiveEditor())) {
			juce::MessageManager::callAsync([editor] {if (editor) { delete editor; }});
		}
	}

	/** Remove Additional Connection */
	this->additionalConnectionList.removeIf(
		[this, nodeID = ptrNode->nodeID](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.nodeID == nodeID) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

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

int PluginDock::getPluginNum() const {
	return this->pluginNodeList.size();
}

PluginDecorator* PluginDock::getPluginProcessor(int index) const {
	if (index < 0 || index >= this->pluginNodeList.size()) { return nullptr; }
	return dynamic_cast<PluginDecorator*>(
		this->pluginNodeList.getUnchecked(index)->getProcessor());
}

void PluginDock::setPluginBypass(int index, bool bypass) {
	if (index < 0 || index >= this->pluginNodeList.size()) { return; }
	if (auto node = this->pluginNodeList.getUnchecked(index)) {
		node->setBypassed(bypass);
	}
}

bool PluginDock::addAdditionalAudioBus() {
	/** Check Channel Num */
	if (this->getTotalNumInputChannels() + this->audioChannels.size() >= juce::AudioProcessorGraph::midiChannelIndex) {
		return false;
	}

	/** Prepare Bus Layout */
	auto layout = this->getBusesLayout();

	/** Set Additional Channel Num */
	layout.inputBuses.clear();
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels() + this->audioChannels.size()));

	/** Set Bus Layout Of Current Graph */
	this->setBusesLayout(layout);

	/** Set Bus Layout Of Input Node */
	juce::AudioProcessorGraph::BusesLayout inputLayout;
	inputLayout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels()));
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	return true;
}

bool PluginDock::removeAdditionalAudioBus() {
	/** Check Channel Num */
	if (this->getTotalNumInputChannels() - this->audioChannels.size() >= this->audioChannels.size()) {
		return false;
	}

	/** Prepare Bus Layout */
	auto layout = this->getBusesLayout();

	/** Set Additional Channel Num */
	layout.inputBuses.clear();
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels() - this->audioChannels.size()));

	/** Set Bus Layout Of Current Graph */
	this->setBusesLayout(layout);

	/** Set Bus Layout Of Input Node */
	juce::AudioProcessorGraph::BusesLayout inputLayout;
	inputLayout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(this->getTotalNumInputChannels()));
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** Auto Remove Connection */
	this->removeIllegalConnections();

	return true;
}

void PluginDock::addAdditionalBusConnection(int pluginIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (pluginIndex < 0 || pluginIndex >= this->pluginNodeList.size()) {
		return;
	}
	if (srcChannel < this->audioChannels.size() ||
		srcChannel >= this->getTotalNumInputChannels()) {
		return;
	}

	/** Get Node ID */
	auto ptrNode = this->pluginNodeList.getUnchecked(pluginIndex);
	auto nodeID = ptrNode->nodeID;

	/** Get Channels */
	int nodeChannels = ptrNode->getProcessor()->getTotalNumInputChannels();
	if (dstChannel < 0 || dstChannel >= nodeChannels) { return; }

	/** Link Bus */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	if (!this->isConnected(connection)) {
		this->addConnection(connection);
		this->additionalConnectionList.add(connection);
	}
}

void PluginDock::removeAdditionalBusConnection(int pluginIndex, int srcChannel, int dstChannel) {
	/** Limit Index */
	if (pluginIndex < 0 || pluginIndex >= this->pluginNodeList.size()) {
		return;
	}

	/** Get Node ID */
	auto nodeID = this->pluginNodeList.getUnchecked(pluginIndex)->nodeID;

	/** Remove Connection */
	juce::AudioProcessorGraph::Connection connection =
	{ {this->audioInputNode->nodeID, srcChannel}, {nodeID, dstChannel} };
	this->removeConnection(connection);
	this->additionalConnectionList.removeAllInstancesOf(connection);
}

PluginDock::PluginStateList PluginDock::getPluginList() const {
	PluginDock::PluginStateList result;

	for (auto& plugin : this->pluginNodeList) {
		result.add(std::make_tuple(
			plugin->getProcessor()->getName(), !plugin->isBypassed()));
	}

	return result;
}

void PluginDock::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** Plugin Dock */
	this->juce::AudioProcessorGraph::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void PluginDock::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessorGraph::setPlayHead(newPlayHead);

	/** Plugins */
	for (auto& i : this->pluginNodeList) {
		auto plugin = i->getProcessor();
		plugin->setPlayHead(newPlayHead);
	}
}

bool PluginDock::parse(const google::protobuf::Message* data) {
	/** TODO */
	return true;
}

std::unique_ptr<const google::protobuf::Message> PluginDock::serialize() const {
	/** TODO */
	return nullptr;
}
