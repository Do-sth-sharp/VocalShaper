#pragma once

#include <Defs.h>

class PluginDock final : public juce::AudioProcessorGraph {
public:
	PluginDock() = delete;
	PluginDock(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());

	void insertPlugin(std::unique_ptr<juce::AudioProcessor> processor, int index = -1);
	void removePlugin(int index);

	void addAdditionalAudioBus();
	void removeAdditionalAudioBus();

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;
	juce::AudioChannelSet audioChannels;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> pluginNodeList;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginDock)
};
