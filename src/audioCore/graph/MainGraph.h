#pragma once

#include <JuceHeader.h>
#include "Track.h"
#include "PluginDecorator.h"
#include "SourceRecordProcessor.h"
#include "../project/Serializable.h"
#include "../Utils.h"

class MainGraph final : public juce::AudioProcessorGraph,
	public Serializable {
public:
	MainGraph();
	~MainGraph() override;

	using TrackType = Track::TrackType;
	void insertTrack(TrackType type, int index = -1,
		const juce::AudioChannelSet& bus = juce::AudioChannelSet::stereo());
	void removeTrack(TrackType type, int index);

	int getTrackNum(TrackType type) const;
	Track* getTrackProcessor(TrackType type, int index) const;

	void setMIDII2SrcConnection(int sourceIndex);
	void removeMIDII2SrcConnection(int sourceIndex);
	void setAudioI2SrcConnection(int sourceIndex, int srcChannel, int dstChannel);
	void removeAudioI2SrcConnection(int sourceIndex, int srcChannel, int dstChannel);
	void setMIDISrc2TrkConnection(int sourceIndex, int trackIndex);
	void removeMIDISrc2TrkConnection(int sourceIndex, int trackIndex);
	void setAudioSrc2TrkConnection(int sourceIndex, int trackIndex, int srcChannel, int dstChannel);
	void removeAudioSrc2TrkConnection(int sourceIndex, int trackIndex, int srcChannel, int dstChannel);
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

	bool isMIDII2SrcConnected(int sourceIndex) const;
	bool isAudioI2SrcConnected(int sourceIndex, int srcChannel, int dstChannel) const;
	bool isMIDISrc2TrkConnected(int sourceIndex, int trackIndex) const;
	bool isAudioSrc2TrkConnected(int sourceIndex, int trackIndex, int srcChannel, int dstChannel) const;
	bool isMIDII2TrkConnected(int trackIndex) const;
	bool isAudioI2TrkConnected(int trackIndex, int srcChannel, int dstChannel) const;
	bool isAudioTrk2OConnected(int trackIndex, int srcChannel, int dstChannel) const;
	bool isAudioTrk2TrkConnected(int trackIndex, int dstTrackIndex, int srcChannel, int dstChannel) const;
	bool isMIDITrk2OConnected(int trackIndex) const;

	utils::AudioConnectionList getTrackInputFromTrackConnections(int index) const;
	utils::AudioConnectionList getTrackInputFromSrcConnections(int index) const;
	utils::AudioConnectionList getTrackInputFromDeviceConnections(int index) const;
	utils::AudioConnectionList getTrackOutputToTrackConnections(int index) const;
	utils::AudioConnectionList getTrackOutputToDeviceConnections(int index) const;

	utils::MidiConnectionList getTrackMidiInputFromSrcConnections(int index) const;
	utils::MidiConnectionList getTrackMidiInputFromDeviceConnections(int index) const;
	utils::MidiConnectionList getTrackMidiOutputToDeviceConnections(int index) const;

	utils::AudioConnectionList getSourceInputFromDeviceConnections(int index) const;
	utils::AudioConnectionList getSourceOutputToTrackConnections(int index) const;

	utils::MidiConnectionList getSourceMidiInputFromDeviceConnections(int index) const;
	utils::MidiConnectionList getSourceMidiOutputToTrackConnections(int index) const;

	/**
	 * @brief	Set the input and output channel number of current audio device.
	 */
	void setAudioLayout(int inputChannelNum, int outputChannelNum);

	/**
	 * @brief	Set the MIDI Message Hook.
	 */
	void setMIDIMessageHook(const std::function<void(const juce::MidiMessage&, bool)> hook);

	using MIDICCListener = std::function<void(int)>;
	void setMIDICCListener(const MIDICCListener& listener);
	void clearMIDICCListener();

	void closeAllNote();

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void setPlayHead(juce::AudioPlayHead* newPlayHead) override;
	double getTailLengthSeconds() const override;

	SourceRecordProcessor* getRecorder() const;

	void clearGraph();

	void updateARAContext();
	const juce::Array<float> getOutputLevels() const;

	void writeRecordingDataToSource(
		double startTime, double currentTime, double sampleRate,
		const juce::MidiMessageSequence& midiData, const juce::AudioSampleBuffer& audioData);

	class SafePointer {
	private:
		juce::WeakReference<MainGraph> weakRef;

	public:
		SafePointer() = default;
		SafePointer(MainGraph* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (MainGraph* newSource) { weakRef = newSource; return *this; };

		MainGraph* getGraph() const noexcept { return dynamic_cast<MainGraph*> (weakRef.get()); };
		operator MainGraph* () const noexcept { return getGraph(); };
		MainGraph* operator->() const noexcept { return getGraph(); };
		void deleteAndZero() { delete getGraph(); };

		bool operator== (MainGraph* component) const noexcept { return weakRef == component; };
		bool operator!= (MainGraph* component) const noexcept { return weakRef != component; };
	};

public:
	bool parse(
		const google::protobuf::Message* data,
		const ParseConfig& config) override;
	std::unique_ptr<google::protobuf::Message> serialize(
		const SerializeConfig& config) const override;

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	std::unique_ptr<SourceRecordProcessor> recorder = nullptr;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> trackList;
	juce::Array<juce::AudioProcessorGraph::Node::Ptr> auxTrackList;
	juce::AudioProcessorGraph::Node::Ptr masterTrack;

	enum class SendDstType {
		ToDevice = 0, ToMaster, ToAUX
	};

	const static int audioSendSlotNum = 4;
	const static int midiSendSlotNum = 2;

	using AudioChannelLink = std::pair<int, int>;
	using AudioChannelLinkList = std::set<AudioChannelLink>;
	using NodeIndex = juce::AudioProcessorGraph::NodeID;
	using AudioSendDstGroup = std::pair<NodeIndex, AudioChannelLinkList>;
	std::set<NodeIndex> midiInputLinks;
	std::map<NodeIndex, AudioChannelLinkList> audioInputLinks;
	std::map<NodeIndex, std::array<NodeIndex, midiSendSlotNum>> midiSendLinks;
	std::map<NodeIndex, std::array<AudioSendDstGroup, audioSendSlotNum>> audioSendLinks;

	std::map<NodeIndex, std::multiset<NodeIndex>> midiSendSrcTemp;
	std::map<NodeIndex, std::multiset<NodeIndex>> audioSendSrcTemp;

	std::function<void(const juce::MidiMessage&, bool)> midiHook;

	MIDICCListener ccListener;

	juce::Array<float> outputLevels;

	mutable double totalLengthTemp = 0;

	bool addMIDIInputLink(NodeIndex track);
	bool addAudioInputLink(NodeIndex track, int inputChannel, int trackChannel);
	bool addMIDISendLink(NodeIndex track, int slot, NodeIndex dst);
	bool addAudioSendLink(NodeIndex track, int slot, NodeIndex dst, int trackChannel, int dstChannel);
	bool removeMIDIInputLink(NodeIndex track);
	bool removeAudioInputLink(NodeIndex track, int inputChannel, int trackChannel);
	const AudioChannelLinkList removeAudioInputLink(NodeIndex track);
	bool removeMIDISendLink(NodeIndex track, int slot, NodeIndex dst);
	NodeIndex removeMIDISendLink(NodeIndex track, int slot);
	bool removeAudioSendLink(NodeIndex track, int slot, NodeIndex dst, int trackChannel, int dstChannel);
	const AudioChannelLinkList removeAudioSendLink(NodeIndex track, int slot, NodeIndex dst);
	const AudioSendDstGroup removeAudioSendLink(NodeIndex track, int slot);
	bool checkMIDIInputLink(NodeIndex track) const;
	bool checkAudioInputLink(NodeIndex track, int inputChannel, int trackChannel) const;
	bool checkAudioInputLink(NodeIndex track) const;
	bool checkMIDISendLink(NodeIndex track, int slot, NodeIndex dst) const;
	bool checkAudioSendLink(NodeIndex track, int slot, NodeIndex dst, int trackChannel, int dstChannel) const;
	bool checkAudioSendLink(NodeIndex track, int slot, NodeIndex dst) const;

	const NodeIndex getMIDISendSlot(NodeIndex track, int slot) const;
	const AudioSendDstGroup getAudioSendSlot(NodeIndex track, int slot) const;
	NodeIndex& getMIDISendSlot(NodeIndex track, int slot);
	AudioSendDstGroup& getAudioSendSlot(NodeIndex track, int slot);

	const std::set<NodeIndex> getMIDISendLinkSrc(NodeIndex track) const;
	const std::set<NodeIndex> getAudioSendLinkSrc(NodeIndex track) const;

	void disconnectTrackMIDIInputLinks(NodeIndex track);
	void disconnectTrackAudioInputLinks(NodeIndex track);
	void disconnectTrackMIDISendLinks(NodeIndex track);
	void disconnectTrackAudioSendLinks(NodeIndex track);
	void disconnectTrackMIDISendInLinks(NodeIndex track);
	void disconnectTrackAudioSendInLinks(NodeIndex track);

	void destoryTrackLinkTemps(NodeIndex track);

	void removeIllegalAudioI2SrcConnections();
	void removeIllegalAudioI2TrkConnections();
	void removeIllegalAudioTrk2OConnections();

	int findSource(const SeqSourceProcessor* ptr) const;
	int findTrack(const Track* ptr) const;

	void ensureMasterTrackOutputLink();
	void initMasterTrack(const juce::AudioChannelSet& bus = juce::AudioChannelSet::stereo());

	friend class Renderer;
	friend class RenderThread;
	void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;

	JUCE_DECLARE_WEAK_REFERENCEABLE(MainGraph)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainGraph)
};
