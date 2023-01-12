#pragma once

#include <Defs.h>

class Sequencer final : public juce::AudioProcessorGraph {
public:
	Sequencer();

private:
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioProcessorGraph::Node::Ptr audioOutputNode;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sequencer)
};
