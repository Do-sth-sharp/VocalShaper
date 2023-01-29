#pragma once

#include <Defs.h>

class Mixer final : public juce::AudioProcessorGraph {
public:
	Mixer();

	void insertTrack(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	void removeTrack(int index);

	void setTrackAudioInputFromSequencer(int trackIndex, int srcChannel, int dstChannel);
	void setTrackAudioInputFromDevice(int trackIndex, int srcChannel, int dstChannel);
	void setTrackAudioOutput(int trackIndex, int srcChannel, int dstChannel);
	void removeTrackAudioInputFromSequencer(int trackIndex, int srcChannel, int dstChannel);
	void removeTrackAudioInputFromDevice(int trackIndex, int srcChannel, int dstChannel);
	void removeTrackAudioOutput(int trackIndex, int srcChannel, int dstChannel);

	void setAudioLayout(const juce::AudioProcessorGraph::BusesLayout& busLayout);

	void removeIllegalInputConnections();
	void removeIllegalOutputConnections();

	void setTrackSend(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel);
	void removeTrackSend(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel);

	void setOutputChannels(const juce::Array<juce::AudioChannelSet>& channels);

	void addSequencerBus(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	void removeSequencerBus();

	void setInputDeviceChannels(const juce::Array<juce::AudioChannelSet>& channels);

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> trackNodeList;

	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioInputFromSequencerConnectionList, trackAudioInputFromDeviceConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioSendConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioOutputConnectionList;

	juce::AudioProcessorGraph::Node::Ptr insertTrackInternal(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	juce::AudioProcessorGraph::Node::Ptr removeTrackInternal(int index);

	int sequencerBusNum = 0;
	int sequencerChannelNum = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
