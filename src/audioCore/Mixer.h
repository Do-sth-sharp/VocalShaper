#pragma once

#include <Defs.h>

class Mixer final : public juce::AudioProcessorGraph {
public:
	Mixer();

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
