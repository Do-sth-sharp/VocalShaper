#pragma once

#include <JuceHeader.h>
#include "Track.h"

class MainGraph final : public juce::AudioProcessorGraph {
public:
	MainGraph();
	~MainGraph() override;

	void insertSource(std::unique_ptr<juce::AudioProcessor> processor, int index = -1);
	void removeSource(int index);
	bool insertInstrument(std::unique_ptr<juce::AudioPluginInstance> processor, int index = -1);
	void removeInstrument(int index);
	void insertTrack(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	void removeTrack(int index);

	int getSourceNum() const;
	juce::AudioProcessor* getSourceProcessor(int index) const;
	int getInstrumentNum() const;
	juce::AudioPluginInstance* getInstrumentProcessor(int index) const;
	int getTrackNum() const;
	Track* getTrackProcessor(int index) const;
	void setInstrumentBypass(int index, bool bypass);

	void setMIDII2SrcConnection(int sourceIndex);
	void removeMIDII2SrcConnection(int sourceIndex);
	void setAudioI2SrcConnection(int sourceIndex, int srcChannel, int dstChannel);
	void removeAudioI2SrcConnection(int sourceIndex, int srcChannel, int dstChannel);
	void addMIDISrc2InstrConnection(int sourceIndex, int instrIndex);
	void removeMIDISrc2InstrConnection(int sourceIndex, int instrIndex);
	void setMIDISrc2TrkConnection(int sourceIndex, int trackIndex);
	void removeMIDISrc2TrkConnection(int sourceIndex, int trackIndex);
	void setAudioSrc2TrkConnection(int sourceIndex, int trackIndex, int srcChannel, int dstChannel);
	void removeAudioSrc2TrkConnection(int sourceIndex, int trackIndex, int srcChannel, int dstChannel);
	void setAudioInstr2TrkConnection(int instrIndex, int trackIndex, int srcChannel, int dstChannel);
	void removeAudioInstr2TrkConnection(int instrIndex, int trackIndex, int srcChannel, int dstChannel);
	void setAudioI2TrkConnection(int trackIndex, int srcChannel, int dstChannel);
	void removeAudioI2TrkConnection(int trackIndex, int srcChannel, int dstChannel);
	void setAudioTrk2OConnection(int trackIndex, int srcChannel, int dstChannel);
	void removeAudioTrk2OConnection(int trackIndex, int srcChannel, int dstChannel);
	void setAudioTrk2TrkConnection(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel);
	void removeAudioTrk2TrkConnection(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel);

	bool isMIDII2SrcConnected(int sourceIndex) const;
	bool isAudioI2SrcConnected(int sourceIndex, int srcChannel, int dstChannel) const;
	bool isMIDISrc2InstrConnected(int sourceIndex, int instrIndex) const;
	bool isMIDISrc2TrkConnected(int sourceIndex, int trackIndex) const;
	bool isAudioSrc2TrkConnected(int sourceIndex, int trackIndex, int srcChannel, int dstChannel) const;
	bool isAudioInstr2TrkConnected(int instrIndex, int trackIndex, int srcChannel, int dstChannel) const;
	bool isAudioI2TrkConnected(int trackIndex, int srcChannel, int dstChannel) const;
	bool isAudioTrk2OConnected(int trackIndex, int srcChannel, int dstChannel) const;
	bool isAudioTrk2TrkConnected(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) const;

	using TrackConnection = std::tuple<int, int, int, int>;
	using TrackConnectionList = juce::Array<TrackConnection>;

	TrackConnectionList getTrackInputFromTrackConnections(int index);
	TrackConnectionList getTrackInputFromSrcConnections(int index);
	TrackConnectionList getTrackInputFromInstrConnections(int index);
	TrackConnectionList getTrackInputFromDeviceConnections(int index);
	TrackConnectionList getTrackOutputToTrackConnections(int index);
	TrackConnectionList getTrackOutputToDeviceConnections(int index);

	/**
	 * @brief	Set the input and output channel number of current audio device.
	 */
	void setAudioLayout(int inputChannelNum, int outputChannelNum);

	/**
	 * @brief	Set the MIDI Message Hook.
	 */
	void setMIDIMessageHook(const std::function<void(const juce::MidiMessage&)> hook);

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> audioSourceNodeList;
	juce::Array<juce::AudioProcessorGraph::Node::Ptr> instrumentNodeList;
	juce::Array<juce::AudioProcessorGraph::Node::Ptr> trackNodeList;

	juce::Array<juce::AudioProcessorGraph::Connection> midiI2SrcConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioI2SrcConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiSrc2InstrConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiSrc2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioSrc2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioInstr2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioI2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioTrk2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioTrk2OConnectionList;

	std::function<void(const juce::MidiMessage&)> midiHook;
	juce::ReadWriteLock hookLock;

	void removeIllegalAudioI2SrcConnections();
	void removeIllegalAudioI2TrkConnections();
	void removeIllegalAudioTrk2OConnections();

	void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainGraph)
};
