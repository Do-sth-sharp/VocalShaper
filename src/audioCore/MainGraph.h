#pragma once

#include <Defs.h>

class MainGraph final : public juce::AudioProcessorGraph {
public:
	MainGraph();

	void setAudioLayout(int inputChannelNum, int outputChannelNum);

private:
	juce::AudioProcessorGraph::Node::Ptr sequencer;
	juce::AudioProcessorGraph::Node::Ptr mixer;
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainGraph)
};
