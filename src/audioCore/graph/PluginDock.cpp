#include "PluginDock.h"
#include "../uiCallback/UICallback.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

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
	int mainBusChannels = type.size();
	for (int i = 0; i < mainBusChannels; i++) {
		this->addConnection(
			{ {this->audioInputNode->nodeID, i}, {this->audioOutputNode->nodeID, i} });
	}
}

PluginDock::~PluginDock() {
	/** Nothing To Do */
}

void PluginDock::updateIndex(int index) {
	this->index = index;
}

PluginDecorator::SafePointer PluginDock::insertPlugin(std::unique_ptr<juce::AudioPluginInstance> processor,
	const juce::String& identifier, int index) {
	if (auto ptr = this->insertPlugin(index)) {
		ptr->setPlugin(std::move(processor), identifier);
		return ptr;
	}
	else {
		jassertfalse;
		return nullptr;
	}
}

PluginDecorator::SafePointer PluginDock::insertPlugin(int index) {
	/** Add To The Graph */
	auto ptrNode = this->addNode(std::make_unique<PluginDecorator>(false, this->audioChannels));
	if (ptrNode) {
		/** Set Bus Num */
		{
			int additionalBusNum = this->getBusCount(true) - 1;
			for (int i = 0; i < additionalBusNum; i++) {
				ptrNode->getProcessor()->addBus(true);
			}
		}

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
			int mainBusChannels = this->audioChannels.size();

			/** Get Plugin Bus */
			int pluginInputChannels = ptrNode->getProcessor()->getMainBusNumInputChannels();
			int pluginOutputChannels = ptrNode->getProcessor()->getMainBusNumOutputChannels();

			/** Check Channels */
			if (pluginInputChannels < mainBusChannels || pluginOutputChannels < mainBusChannels) {
				this->removeNode(ptrNode->nodeID);
				return nullptr;
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

		/** Callback */
		UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, this->index, index);

		return PluginDecorator::SafePointer{ dynamic_cast<PluginDecorator*>(ptrNode->getProcessor()) };
	}
	else {
		jassertfalse;
		return nullptr;
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
		int mainBusChannels = this->audioChannels.size();

		/** Add Connection Between Hot Spot Nodes */
		for (int i = 0; i < mainBusChannels; i++) {
			this->addConnection(
				{ {lastNode->nodeID, i}, {nextNode->nodeID, i} });
		}
	}

	/** Callback */
	UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, this->index, index);
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
		PluginDock::setPluginBypass(PluginDecorator::SafePointer{
			dynamic_cast<PluginDecorator*>(node->getProcessor()) }, bypass);
	}
}

bool PluginDock::getPluginBypass(int index) const {
	if (index < 0 || index >= this->pluginNodeList.size()) { return false; }
	if (auto node = this->pluginNodeList.getUnchecked(index)) {
		return PluginDock::getPluginBypass(PluginDecorator::SafePointer{
			dynamic_cast<PluginDecorator*>(node->getProcessor()) });
	}
	return false;
}

void PluginDock::setPluginBypass(PluginDecorator::SafePointer plugin, bool bypass) {
	if (plugin) {
		if (auto bypassParam = plugin->getBypassParameter()) {
			bypassParam->setValueNotifyingHost(bypass ? 1.0f : 0.0f);

			/** Callback */
			UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, -1, -1);
		}
	}
}

bool PluginDock::getPluginBypass(PluginDecorator::SafePointer plugin) {
	if (plugin) {
		if (auto bypassParam = plugin->getBypassParameter()) {
			return !juce::approximatelyEqual(bypassParam->getValue(), 0.0f);
		}
	}
	return false;
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

	/** Set Bus Num Of Plugins */
	for (auto& p : this->pluginNodeList) {
		p->getProcessor()->addBus(true);
	}

	return true;
}

bool PluginDock::removeAdditionalAudioBus() {
	/** Check Channel Num */
	if (this->getTotalNumInputChannels() - this->audioChannels.size() < this->audioChannels.size()) {
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

	/** Set Bus Num Of Plugins */
	for (auto& p : this->pluginNodeList) {
		p->getProcessor()->removeBus(true);
	}

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

	/** Callback */
	UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, this->index, pluginIndex);
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

	/** Callback */
	UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, this->index, pluginIndex);
}

bool PluginDock::isAdditionalBusConnected(int pluginIndex, int srcChannel, int dstChannel) const {
	/** Limit Index */
	if (pluginIndex < 0 || pluginIndex >= this->pluginNodeList.size()) {
		return false;
	}

	/** Get Node ID */
	auto nodeID = this->pluginNodeList.getUnchecked(pluginIndex)->nodeID;

	/** Check Connection */
	for (auto& i : this->additionalConnectionList) {
		if (i.source.channelIndex == srcChannel &&
			i.destination.nodeID == nodeID &&
			i.destination.channelIndex == dstChannel) {
			return true;
		}
	}

	return false;
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

void PluginDock::clearGraph() {
	for (auto& i : this->additionalConnectionList) {
		this->removeConnection(i);
	}
	this->additionalConnectionList.clear();

	for (auto& i : this->pluginNodeList) {
		this->removeNode(i->nodeID);
	}
	this->pluginNodeList.clear();

	this->removeIllegalConnections();

	int mainBusChannels = this->audioChannels.size();
	for (int i = 0; i < mainBusChannels; i++) {
		this->addConnection(
			{ {this->audioInputNode->nodeID, i}, {this->audioOutputNode->nodeID, i} });
	}

	/** Callback */
	UICallbackAPI<int, int>::invoke(UICallbackType::EffectChanged, this->index, -1);
}

utils::AudioConnectionList PluginDock::getPluginAdditionalBusConnections(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->pluginNodeList.size()) {
		return utils::AudioConnectionList{};
	}

	/** Get Current Track ID */
	juce::AudioProcessorGraph::NodeID currentID
		= this->pluginNodeList.getUnchecked(index)->nodeID;
	utils::AudioConnectionList resultList;

	for (auto& i : this->additionalConnectionList) {
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

bool PluginDock::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::PluginDock*>(data);
	if (!mes) { return false; }

	this->clearGraph();

	auto& plugins = mes->plugins();
	for (auto& i : plugins) {
		this->insertPlugin(-1);
		if (auto pluginNode = this->pluginNodeList.getLast()) {
			if (auto plugin = dynamic_cast<PluginDecorator*>(pluginNode->getProcessor())) {
				PluginDock::setPluginBypass(PluginDecorator::SafePointer{ plugin }, i.bypassed());
				if (!plugin->parse(&i)) { return false; }
			}
		}
	}

	auto& connections = mes->connections();
	for (auto& i : connections) {
		this->addAdditionalBusConnection(i.dst(), i.srcchannel(), i.dstchannel());
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> PluginDock::serialize() const {
	auto mes = std::make_unique<vsp4::PluginDock>();

	auto plugins = mes->mutable_plugins();
	for (auto& i : this->pluginNodeList) {
		if (auto plugin = dynamic_cast<PluginDecorator*>(i->getProcessor())) {
			if (auto item = plugin->serialize()) {
				if (auto plu = dynamic_cast<vsp4::Plugin*>(item.get())) {
					plu->set_bypassed(PluginDock::getPluginBypass(
						PluginDecorator::SafePointer{ plugin }));

					plugins->AddAllocated(dynamic_cast<vsp4::Plugin*>(item.release()));
				}
			}
			else {
				return nullptr;
			}
		}
	}

	auto connections = mes->mutable_connections();
	for (auto& i : this->additionalConnectionList) {
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioInputConnection>();
		cmes->set_dst(this->findPlugin(dynamic_cast<PluginDecorator*>(dstNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		connections->AddAllocated(cmes.release());
	}

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

int PluginDock::findPlugin(const PluginDecorator* ptr) const {
	for (int i = 0; i < this->pluginNodeList.size(); i++) {
		if (this->pluginNodeList.getUnchecked(i)->getProcessor() == ptr) {
			return i;
		}
	}
	return -1;
}
