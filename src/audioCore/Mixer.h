#pragma once

#include <Defs.h>

class Mixer final : public juce::AudioProcessorGraph {
public:
	Mixer();

	void insertTrack(int index = -1);
	void removeTrack(int index);

	void setTrackAudioInput(int trackIndex, int busIndex);
	void setTrackAudioOutput(int trackIndex, int busIndex);
	void removeTrackAudioInput(int trackIndex);
	void removeTrackAudioOutput(int trackIndex);

	void setAudioLayout(const juce::AudioProcessorGraph::BusesLayout& busLayout);

	void removeIllegalInputConnections();
	void removeIllegalOutputConnections();

	void setTrackSend(int trackIndex, int dstTrackIndex);
	void addTrackSideLink(int trackIndex, int dstTrackIndex, int dstBus);
	void removeTrackSideLink(int trackIndex, int dstTrackIndex);

	void setOutputChannels(const juce::Array<juce::AudioChannelSet>& channels);

	void addSequencerBus();
	void removeSequencerBus();

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> trackNodeList;

	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioSendConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioSideLinkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioInputConnectionList, trackAudioOutputConnectionList;

	juce::AudioProcessorGraph::Node::Ptr insertTrackInternal(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	juce::AudioProcessorGraph::Node::Ptr removeTrackInternal(int index);

	int sequencerBusNum = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
