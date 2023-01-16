#pragma once

#include <Defs.h>

class Mixer final : public juce::AudioProcessorGraph {
public:
	Mixer();

	void insertTrack(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	void removeTrack(int index);

	void setTrackAudioInput(int trackIndex, int busIndex);
	void setTrackAudioOutput(int trackIndex, int busIndex);
	void removeTrackAudioInput(int trackIndex);
	void removeTrackAudioOutput(int trackIndex);

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> trackNodeList;

	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioInputConnectionList, trackAudioOutputConnectionList;

	juce::AudioProcessorGraph::Node::Ptr insertTrackInternal(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	juce::AudioProcessorGraph::Node::Ptr removeTrackInternal(int index);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
