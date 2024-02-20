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
			auto busLayout = this->getBusesLayout();
			ptrNode->getProcessor()->setBusesLayout(busLayout);
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

		/** Connect Additional Bus */
		for (int i = this->audioChannels.size(); i < this->getNumInputChannels(); i++) {
			juce::AudioProcessorGraph::Connection connection =
			{ {this->audioInputNode->nodeID, i}, {ptrNode->nodeID, i} };
			this->addConnection(connection);
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
	for (int i = this->audioChannels.size(); i < this->getNumInputChannels(); i++) {
		juce::AudioProcessorGraph::Connection connection =
		{ {this->audioInputNode->nodeID, i}, {ptrNode->nodeID, i} };
		this->removeConnection(connection);
	}

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
	int oldNum = this->getTotalNumInputChannels();
	if (oldNum + this->audioChannels.size() >= juce::AudioProcessorGraph::midiChannelIndex) {
		return false;
	}

	/** Prepare Bus Layout */
	auto layout = this->getBusesLayout();

	/** Set Additional Channel Num */
	layout.inputBuses.clear();
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(oldNum + this->audioChannels.size()));
	layout.outputBuses.clear();
	layout.outputBuses.add(
		juce::AudioChannelSet::discreteChannels(oldNum + this->audioChannels.size()));

	/** Set Bus Layout Of Current Graph */
	this->setBusesLayout(layout);

	/** Set Bus Layout Of Input Node */
	int newNum = this->getTotalNumInputChannels();
	this->audioInputNode->getProcessor()->setBusesLayout(layout);
	this->audioOutputNode->getProcessor()->setBusesLayout(layout);

	/** Set Bus Num Of Plugins */
	for (auto& p : this->pluginNodeList) {
		p->getProcessor()->setBusesLayout(layout);
	}

	/** Connect Additional Bus */
	for (auto& p : this->pluginNodeList) {
		for (int i = oldNum; i < newNum; i++) {
			juce::AudioProcessorGraph::Connection connection =
			{ {this->audioInputNode->nodeID, i}, {p->nodeID, i} };
			this->addConnection(connection);
		}
	}
	for (int i = oldNum; i < newNum; i++) {
		juce::AudioProcessorGraph::Connection connection =
		{ {this->audioInputNode->nodeID, i},
			{this->audioOutputNode->nodeID, i} };
		this->addConnection(connection);
	}

	return true;
}

bool PluginDock::removeAdditionalAudioBus() {
	/** Check Channel Num */
	int oldNum = this->getTotalNumInputChannels();
	if (oldNum - this->audioChannels.size() < this->audioChannels.size()) {
		return false;
	}

	/** Prepare Bus Layout */
	auto layout = this->getBusesLayout();

	/** Set Additional Channel Num */
	layout.inputBuses.clear();
	layout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(oldNum - this->audioChannels.size()));

	/** Set Bus Layout Of Current Graph */
	this->setBusesLayout(layout);

	/** Set Bus Layout Of Input Node */
	int newNum = this->getTotalNumInputChannels();
	juce::AudioProcessorGraph::BusesLayout inputLayout;
	inputLayout.inputBuses.add(
		juce::AudioChannelSet::discreteChannels(newNum));
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** Set Bus Num Of Plugins */
	for (auto& p : this->pluginNodeList) {
		p->getProcessor()->setBusesLayout(layout);
	}

	/** Remove Additional Connection */
	this->removeIllegalConnections();

	return true;
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
