#pragma once

#include <JuceHeader.h>

#include "PluginDecorator.h"
#include "../project/Serializable.h"
#include "../Utils.h"

class PluginDock final : public juce::AudioProcessorGraph,
	public Serializable {
public:
	PluginDock() = delete;
	PluginDock(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	~PluginDock() override;
	
	/**
	 * @brief	Insert a plugin onto the plugin dock.
	 */
	PluginDecorator::SafePointer insertPlugin(std::unique_ptr<juce::AudioPluginInstance> processor,
		const juce::String& identifier, int index = -1);
	/**
	 * @brief	Insert a plugin onto the plugin dock without init.
	 */
	PluginDecorator::SafePointer insertPlugin(int index = -1);
	/**
	 * @brief	Remove a plugin from the plugin dock.
	 */
	void removePlugin(int index);

	int getPluginNum() const;
	PluginDecorator* getPluginProcessor(int index) const;
	void setPluginBypass(int index, bool bypass);
	bool getPluginBypass(int index) const;
	static void setPluginBypass(PluginDecorator::SafePointer plugin, bool bypass);
	static bool getPluginBypass(PluginDecorator::SafePointer plugin);

	/**
	 * @brief	Add an audio input bus onto the plugin dock.
	 */
	bool addAdditionalAudioBus();
	/**
	 * @brief	Remove the last audio input bus from the plugin dock.
	 */
	bool removeAdditionalAudioBus();

	void addAdditionalBusConnection(int pluginIndex, int srcChannel, int dstChannel);
	void removeAdditionalBusConnection(int pluginIndex, int srcChannel, int dstChannel);
	bool isAdditionalBusConnected(int pluginIndex, int srcChannel, int dstChannel) const;

	using PluginState = std::tuple<juce::String, bool>;
	using PluginStateList = juce::Array<PluginState>;
	PluginStateList getPluginList() const;

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void setPlayHead(juce::AudioPlayHead* newPlayHead) override;

	void clearGraph();

	utils::AudioConnectionList getPluginAdditionalBusConnections(int index) const;

	class SafePointer {
	private:
		juce::WeakReference<PluginDock> weakRef;

	public:
		SafePointer() = default;
		SafePointer(PluginDock* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (PluginDock* newSource) { weakRef = newSource; return *this; };

		PluginDock* getDock() const noexcept { return dynamic_cast<PluginDock*> (weakRef.get()); };
		operator PluginDock* () const noexcept { return getDock(); };
		PluginDock* operator->() const noexcept { return getDock(); };
		void deleteAndZero() { delete getDock(); };

		bool operator== (PluginDock* component) const noexcept { return weakRef == component; };
		bool operator!= (PluginDock* component) const noexcept { return weakRef != component; };
	};

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;
	juce::AudioChannelSet audioChannels;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> pluginNodeList;

	juce::Array<juce::AudioProcessorGraph::Connection> additionalConnectionList;

	int findPlugin(const PluginDecorator* ptr) const;

	JUCE_DECLARE_WEAK_REFERENCEABLE(PluginDock)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginDock)
};
