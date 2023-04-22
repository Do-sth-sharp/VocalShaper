#pragma once

#include <JuceHeader.h>
#include "Track.h"

class Mixer final : public juce::AudioProcessorGraph {
public:
	Mixer();

	/**
	 * @brief	Insert a track onto the mixer.
	 */
	void insertTrack(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	/**
	 * @brief	Remove a track from the mixer.
	 */
	void removeTrack(int index);

	/**
	 * @brief	Get track number of the mixer.
	 */
	int getTrackNum() const;
	/**
	 * @brief	Get processor pointer of the track.
	 */
	Track* getTrackProcessor(int index) const;

	/**
	 * @brief	Connect audio input channel of the track with audio input node (sequencer channel).
	 */
	void setTrackAudioInputFromSequencer(int trackIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Connect audio input channel of the track with audio input node (device channel).
	 */
	void setTrackAudioInputFromDevice(int trackIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Connect audio output channel of the track with audio output node.
	 */
	void setTrackAudioOutput(int trackIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Disconnect audio input channel of the track with audio input node (sequencer channel).
	 */
	void removeTrackAudioInputFromSequencer(int trackIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Disconnect audio input channel of the track with audio input node (device channel).
	 */
	void removeTrackAudioInputFromDevice(int trackIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Disconnect audio output channel of the track with audio output node.
	 */
	void removeTrackAudioOutput(int trackIndex, int srcChannel, int dstChannel);

	/**
	 * @brief	Check audio input channel of the track with audio input node (sequencer channel) connection.
	 */
	bool isTrackAudioInputFromSequencer(int trackIndex, int srcChannel, int dstChannel) const;
	/**
	 * @brief	Check audio input channel of the track with audio input node (device channel) connection.
	 */
	bool isTrackAudioInputFromDevice(int trackIndex, int srcChannel, int dstChannel) const;
	/**
	 * @brief	Check audio output channel of the track with audio output node connection.
	 */
	bool isTrackAudioOutput(int trackIndex, int srcChannel, int dstChannel) const;

	/**
	 * @brief	Connect audio output channel of the track with audio input channel of another track.
	 */
	void setTrackSend(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel);
	/**
	 * @brief	Disconnect audio output channel of the track with audio input channel of another track.
	 */
	void removeTrackSend(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel);

	/**
	 * @brief	Check audio output channel of the track with audio input channel of another track connection.
	 */
	bool isTrackSend(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) const;

	/**
	 * @brief	Set audio output channel layout.
	 */
	void setOutputChannels(const juce::Array<juce::AudioChannelSet>& channels);

	/**
	 * @brief	Add a sequencer audio input bus.
	 */
	void addSequencerBus(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	/**
	 * @brief	Remove the last sequencer audio input bus.
	 */
	void removeSequencerBus();

	/**
	 * @brief	Get sequencer audio input bus number.
	 */
	int getSequencerBusNum() const;

	/**
	 * @brief	Get sequencer audio input channel number.
	 */
	int getSequencerChannelNum() const;

	/**
	 * @brief	Set audio input channel (device channel) layout.
	 */
	void setInputDeviceChannels(const juce::Array<juce::AudioChannelSet>& channels);

	using TrackConnection = std::tuple<int, int, int, int>;
	using TrackConnectionList = juce::Array< TrackConnection>;

	TrackConnectionList getTrackInputFromTrackConnections(int index);
	TrackConnectionList getTrackInputFromSequencerConnections(int index);
	TrackConnectionList getTrackInputFromDeviceConnections(int index);
	TrackConnectionList getTrackOutputToTrackConnections(int index);
	TrackConnectionList getTrackOutputToDeviceConnections(int index);

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> trackNodeList;

	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioInputFromSequencerConnectionList, trackAudioInputFromDeviceConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioSendConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> trackAudioOutputConnectionList;

	juce::AudioProcessorGraph::Node::Ptr insertTrackInternal(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	juce::AudioProcessorGraph::Node::Ptr removeTrackInternal(int index);

	void setAudioLayout(const juce::AudioProcessorGraph::BusesLayout& busLayout);

	void removeIllegalInputConnections();
	void removeIllegalOutputConnections();

	int sequencerBusNum = 0;
	int sequencerChannelNum = 0;

	void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
