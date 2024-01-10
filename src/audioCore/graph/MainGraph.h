#pragma once

#include <JuceHeader.h>
#include "Track.h"
#include "PluginDecorator.h"
#include "SeqSourceProcessor.h"
#include "SourceRecordProcessor.h"
#include "../project/Serializable.h"
#include "../Utils.h"

class MainGraph final : public juce::AudioProcessorGraph,
	public Serializable {
public:
	MainGraph();
	~MainGraph() override;

	void insertSource(int index = -1, const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	void removeSource(int index);
	PluginDecorator::SafePointer insertInstrument(
		std::unique_ptr<juce::AudioPluginInstance> processor,
		const juce::String& identifier, int index = -1,
		const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	PluginDecorator::SafePointer insertInstrument(int index = -1,
		const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
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
	void setTrackBypass(int index, bool bypass);
	bool getTrackBypass(int index) const;

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

	utils::AudioConnectionList getTrackInputFromTrackConnections(int index) const;
	utils::AudioConnectionList getTrackInputFromSrcConnections(int index) const;
	utils::AudioConnectionList getTrackInputFromInstrConnections(int index) const;
	utils::AudioConnectionList getTrackInputFromDeviceConnections(int index) const;
	utils::AudioConnectionList getTrackOutputToTrackConnections(int index) const;
	utils::AudioConnectionList getTrackOutputToDeviceConnections(int index) const;

	utils::MidiConnectionList getTrackMidiInputFromSrcConnections(int index) const;
	utils::MidiConnectionList getTrackMidiInputFromDeviceConnections(int index) const;
	utils::MidiConnectionList getTrackMidiOutputToDeviceConnections(int index) const;

	utils::AudioConnectionList getInstrOutputToTrackConnections(int index) const;

	utils::MidiConnectionList getInstrMidiInputFromSrcConnections(int index) const;
	utils::MidiConnectionList getInstrMidiInputFromDeviceConnections(int index) const;

	utils::AudioConnectionList getSourceOutputToTrackConnections(int index) const;

	utils::MidiConnectionList getSourceMidiOutputToInstrConnections(int index) const;
	utils::MidiConnectionList getSourceMidiOutputToTrackConnections(int index) const;

	/**
	 * @brief	Set the input and output channel number of current audio device.
	 */
	void setAudioLayout(int inputChannelNum, int outputChannelNum);

	/**
	 * @brief	Set the MIDI Message Hook.
	 */
	void setMIDIMessageHook(const std::function<void(const juce::MidiMessage&, bool)> hook);

	void closeAllNote();

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void setPlayHead(juce::AudioPlayHead* newPlayHead) override;
	double getTailLengthSeconds() const override;

	SourceRecordProcessor* getRecorder() const;

	void clearGraph();

	const juce::Array<float> getOutputLevels() const;

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

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

	juce::Array<float> outputLevels;
	juce::ReadWriteLock levelsLock;

	void removeIllegalAudioI2TrkConnections();
	void removeIllegalAudioTrk2OConnections();

	int findSource(const SeqSourceProcessor* ptr) const;
	int findInstr(const PluginDecorator* ptr) const;
	int findTrack(const Track* ptr) const;

	friend class Renderer;
	friend class RenderThread;
	void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainGraph)
};
