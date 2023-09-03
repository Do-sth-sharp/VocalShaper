#pragma once

#include <JuceHeader.h>
#include "Track.h"
#include "PluginDecorator.h"
#include "SeqSourceProcessor.h"

class MainGraph final : public juce::AudioProcessorGraph {
public:
	MainGraph();
	~MainGraph() override;

	void insertSource(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	void removeSource(int index);
	bool insertInstrument(std::unique_ptr<juce::AudioPluginInstance> processor, int index = -1);
	void removeInstrument(int index);
	void insertTrack(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	void removeTrack(int index);

	int getSourceNum() const;
	SeqSourceProcessor* getSourceProcessor(int index) const;
	int getInstrumentNum() const;
	PluginDecorator* getInstrumentProcessor(int index) const;
	int getTrackNum() const;
	Track* getTrackProcessor(int index) const;
	void setSourceBypass(int index, bool bypass);
	bool getSourceBypass(int index) const;
	void setInstrumentBypass(int index, bool bypass);
	bool getInstrumentBypass(int index) const;

	void setMIDII2InstrConnection(int instrIndex);
	void removeMIDII2InstrConnection(int instrIndex);
	void setMIDISrc2InstrConnection(int sourceIndex, int instrIndex);
	void removeMIDISrc2InstrConnection(int sourceIndex, int instrIndex);
	void setMIDISrc2TrkConnection(int sourceIndex, int trackIndex);
	void removeMIDISrc2TrkConnection(int sourceIndex, int trackIndex);
	void setAudioSrc2TrkConnection(int sourceIndex, int trackIndex, int srcChannel, int dstChannel);
	void removeAudioSrc2TrkConnection(int sourceIndex, int trackIndex, int srcChannel, int dstChannel);
	void setAudioInstr2TrkConnection(int instrIndex, int trackIndex, int srcChannel, int dstChannel);
	void removeAudioInstr2TrkConnection(int instrIndex, int trackIndex, int srcChannel, int dstChannel);
	void setMIDII2TrkConnection(int trackIndex);
	void removeMIDII2TrkConnection(int trackIndex);
	void setAudioI2TrkConnection(int trackIndex, int srcChannel, int dstChannel);
	void removeAudioI2TrkConnection(int trackIndex, int srcChannel, int dstChannel);
	void setAudioTrk2OConnection(int trackIndex, int srcChannel, int dstChannel);
	void removeAudioTrk2OConnection(int trackIndex, int srcChannel, int dstChannel);
	void setAudioTrk2TrkConnection(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel);
	void removeAudioTrk2TrkConnection(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel);
	void setMIDITrk2OConnection(int trackIndex);
	void removeMIDITrk2OConnection(int trackIndex);

	bool isMIDII2InstrConnected(int instrIndex) const;
	bool isMIDISrc2InstrConnected(int sourceIndex, int instrIndex) const;
	bool isMIDISrc2TrkConnected(int sourceIndex, int trackIndex) const;
	bool isAudioSrc2TrkConnected(int sourceIndex, int trackIndex, int srcChannel, int dstChannel) const;
	bool isAudioInstr2TrkConnected(int instrIndex, int trackIndex, int srcChannel, int dstChannel) const;
	bool isMIDII2TrkConnected(int trackIndex) const;
	bool isAudioI2TrkConnected(int trackIndex, int srcChannel, int dstChannel) const;
	bool isAudioTrk2OConnected(int trackIndex, int srcChannel, int dstChannel) const;
	bool isAudioTrk2TrkConnected(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) const;
	bool isMIDITrk2OConnected(int trackIndex) const;

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
	void setMIDIMessageHook(const std::function<void(const juce::MidiMessage&, bool)> hook);

	void setMIDIOutput(juce::MidiOutput* output);

	void closeAllNote();

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void setPlayHead(juce::AudioPlayHead* newPlayHead) override;

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioProcessorGraph::Node::Ptr recorderNode;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> audioSourceNodeList;
	juce::Array<juce::AudioProcessorGraph::Node::Ptr> instrumentNodeList;
	juce::Array<juce::AudioProcessorGraph::Node::Ptr> trackNodeList;

	juce::Array<juce::AudioProcessorGraph::Connection> midiI2InstrConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiSrc2InstrConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiSrc2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioSrc2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioInstr2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiI2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioI2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioTrk2TrkConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioTrk2OConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiTrk2OConnectionList;

	std::function<void(const juce::MidiMessage&, bool)> midiHook;
	juce::ReadWriteLock hookLock;

	juce::MidiOutput* midiOutput = nullptr;
	juce::ReadWriteLock midiLock;

	void removeIllegalAudioI2TrkConnections();
	void removeIllegalAudioTrk2OConnections();

	void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainGraph)
};
