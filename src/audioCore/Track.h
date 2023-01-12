#pragma once

#include <Defs.h>

class Track final : public juce::AudioProcessorGraph {
public:
	Track() = delete;
	Track(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioChannelSet audioChannels;
};
