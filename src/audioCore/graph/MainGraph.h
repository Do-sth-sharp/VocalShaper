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
	int insertTrack(TrackType type, int index = -1,
		const juce::AudioChannelSet& bus = juce::AudioChannelSet::stereo());
	void removeTrack(TrackType type, int index);

	int getTrackNum(TrackType type) const;
	Track* getTrackProcessor(TrackType type, int index) const;

	bool addTrackAdditionalAudioBus(TrackType type, int index);
	bool removeTrackAdditionalAudioBus(TrackType type, int index);
	int getTrackAdditionalAudioBusNum(TrackType type, int index) const;

	enum class SendDstType {
		ToDevice = 0, ToMaster, ToAUX
	};

	const static int getMIDISendSlotNum();
	const static int getAudioSendSlotNum();

	bool connectTrackMIDIInput(TrackType type, int index);
	bool connectTrackAudioInput(TrackType type, int index, int inputChannel, int trackChannel);
	bool connectTrackMIDISend(TrackType type, int index, int slot, SendDstType dstType, int dstIndex);
	bool connectTrackAudioSend(TrackType type, int index, int slot,
		SendDstType dstType, int dstIndex, int trackChannel, int dstChannel);
	bool disconnectTrackMIDIInput(TrackType type, int index);
	bool disconnectTrackAudioInput(TrackType type, int index, int inputChannel, int trackChannel);
	bool disconnectTrackMIDISend(TrackType type, int index, int slot, SendDstType dstType, int dstIndex);
	bool disconnectTrackMIDISend(TrackType type, int index, int slot);
	bool disconnectTrackAudioSend(TrackType type, int index, int slot,
		SendDstType dstType, int dstIndex, int trackChannel, int dstChannel);
	bool disconnectTrackAudioSend(TrackType type, int index, int slot,
		SendDstType dstType, int dstIndex);
	bool disconnectTrackAudioSend(TrackType type, int index, int slot);

	using AudioChannelLink = std::pair<int, int>;
	using AudioChannelLinkList = std::set<AudioChannelLink>;
	using SendDst = std::pair<SendDstType, int>;
	using TrackIndex = std::pair<TrackType, int>;
	bool isTrackMIDIInputConnected(TrackType type, int index) const;
	bool isTrackAudioInputConnected(TrackType type, int index) const;
	const AudioChannelLinkList getTrackAudioInputChannels(TrackType type, int index) const;
	bool isTrackMIDISendConnected(TrackType type, int index, int slot) const;
	const SendDst getTrackMIDISendDst(TrackType type, int index, int slot) const;
	bool isTrackAudioSendConnected(TrackType type, int index, int slot) const;
	const SendDst getTrackAudioSendDst(TrackType type, int index, int slot) const;
	const AudioChannelLinkList getTrackAudioSendChannels(TrackType type, int index, int slot) const;

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

	NodeIndex getTrackNodeIndex(TrackType type, int index) const;
	NodeIndex getDstNodeIndex(SendDstType type, int index, bool isMIDI) const;

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

	const AudioChannelLinkList getAudioInputChannels(NodeIndex track) const;
	AudioChannelLinkList& getAudioInputChannels(NodeIndex track);

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

	void disconnectTrackLinks(NodeIndex track);
	void destoryTrackLinkTemps(NodeIndex track);

	void removeIllegalAudioInputConnections();
	void removeIllegalAudioOutputConnections();

	void removeIllegalMasterTrackAudioInputConnections();
	void removeIllegalAuxTrackAudioInputConnections(int index);
	void removeIllegalTrackAudioInputConnections(int index);
	void removeIllegalMasterTrackAudioOutputConnections();
	void removeIllegalAuxTrackAudioOutputConnections(int index);
	void removeIllegalTrackAudioOutputConnections(int index);

	void removeIllegalNodeAudioSendInputConnections(NodeIndex track, int inputChannelNum);
	void removeIllegalNodeAudioInputConnections(NodeIndex track);
	void removeIllegalNodeAudioOutputConnections(NodeIndex track);

	const TrackIndex findTrack(NodeIndex track) const;
	const SendDst findDst(NodeIndex dst) const;

	void ensureMasterTrackOutputLink();
	void initMasterTrack(const juce::AudioChannelSet& bus = juce::AudioChannelSet::stereo());

	friend class Renderer;
	friend class RenderThread;
	void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;

	JUCE_DECLARE_WEAK_REFERENCEABLE(MainGraph)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainGraph)
};
