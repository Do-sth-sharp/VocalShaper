#pragma once

#include <Defs.h>

class Channel final : public juce::AudioProcessorGraph {
public:
	Channel() = delete;
	Channel(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());

private:
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioProcessorGraph::Node::Ptr audioOutputNode;
	juce::AudioChannelSet audioChannels;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Channel)
};
