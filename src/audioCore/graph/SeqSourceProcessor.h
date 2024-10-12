#pragma once

#include <JuceHeader.h>

#include "SourceList.h"
#include "PluginDecorator.h"
#include "../project/Serializable.h"

class SeqSourceProcessor final : public juce::AudioProcessorGraph,
	public Serializable {
public:
	SeqSourceProcessor() = delete;
	SeqSourceProcessor(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	~SeqSourceProcessor();

	void updateIndex(int index);

	int addSeq(const SourceList::SeqBlock& block);
	void removeSeq(int index);
	int getSeqNum() const;
	const SourceList::SeqBlock getSeq(int index) const;
	bool splitSeq(int index, double time);
	bool stickSeqWithNext(int index);
	int resetSeqTime(int index, const SourceList::SeqBlock& block);

	void setTrackName(const juce::String& name);
	const juce::String getTrackName() const;
	void setTrackColor(const juce::Colour& color);
	const juce::Colour getTrackColor() const;

	const juce::AudioChannelSet& getAudioChannelSet() const;

	void closeAllNote();

	void setInstr(std::unique_ptr<juce::AudioPluginInstance> processor,
		const juce::String& identifier);
	PluginDecorator::SafePointer prepareInstr();
	void removeInstr();
	PluginDecorator* getInstrProcessor() const;
	void setInstrumentBypass(bool bypass);
	bool getInstrumentBypass() const;
	static void setInstrumentBypass(PluginDecorator::SafePointer instr, bool bypass);
	static bool getInstrumentBypass(PluginDecorator::SafePointer instr);
	void setInstrOffline(bool offline);
	bool getInstrOffline() const;

	uint64_t getAudioRef() const;
	uint64_t getMIDIRef() const;

	void applyAudio();
	void applyMIDI();
	void releaseAudio();
	void releaseMIDI();
	void applyAudioIfNeed();
	void applyMIDIIfNeed();

	const juce::String getAudioName() const;
	const juce::String getMIDIName() const;
	double getMIDILength() const;
	double getAudioLength() const;

	double getAudioSampleRate() const;

	bool isSourceInfoValid() const;
	double getAudioSampleRateTemped() const;
	double getAudioLengthTemped() const;

	void setCurrentMIDITrack(int trackIndex);
	int getCurrentMIDITrack() const;
	int getTotalMIDITrackNum() const;

	void setRecording(bool recording);
	bool getRecording() const;

	void setMute(bool mute);
	bool getMute() const;

	const juce::Array<float> getOutputLevels() const;

	void syncARAContext();

public:
	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void releaseResources() override {};
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	double getTailLengthSeconds() const override;

	void clearGraph();

	class SafePointer {
	private:
		juce::WeakReference<SeqSourceProcessor> weakRef;

	public:
		SafePointer() = default;
		SafePointer(SeqSourceProcessor* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (SeqSourceProcessor* newSource) { weakRef = newSource; return *this; };

		SeqSourceProcessor* getSourceSeq() const noexcept { return dynamic_cast<SeqSourceProcessor*> (weakRef.get()); };
		operator SeqSourceProcessor* () const noexcept { return getSourceSeq(); };
		SeqSourceProcessor* operator->() const noexcept { return getSourceSeq(); };
		void deleteAndZero() { delete getSourceSeq(); };

		bool operator== (SeqSourceProcessor* component) const noexcept { return weakRef == component; };
		bool operator!= (SeqSourceProcessor* component) const noexcept { return weakRef != component; };
	};

public:
	bool parse(
		const google::protobuf::Message* data,
		const ParseConfig& config) override;
	std::unique_ptr<google::protobuf::Message> serialize(
		const SerializeConfig& config) const override;

private:
	std::atomic_int index = -1;

	const juce::AudioChannelSet audioChannels;

	SourceList srcs;
	std::set<std::tuple<int, int>> activeNoteSet;
	std::atomic_bool noteCloseFlag = false;

	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioProcessorGraph::Node::Ptr instr = nullptr;
	std::atomic_bool instrOffline = false;

	juce::String trackName;
	juce::Colour trackColor;

	uint64_t audioSourceRef = 0, midiSourceRef = 0;
	std::atomic_int currentMIDITrack = 0;

	std::atomic_bool recordingFlag = false;

	std::atomic_bool isMute = false;

	juce::Array<float> outputLevels;

	struct SourceInfo final {
		double audioSampleRate = 0;
		double audioLength = 0;
	};
	/** To Make ARA Happy.Only Valid On Current Track Has Audio Source And Audio Loading Not Completed */
	SourceInfo sourceInfo{};
	bool sourceInfoValid = false;

	friend class SourceRecordProcessor;
	void readAudioData(juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	void readMIDIData(juce::MidiBuffer& buffer, int baseTime,
		int startTime, int endTime) const;
	void writeAudioData(juce::AudioBuffer<float>& buffer, int offset);
	void writeMIDIData(const juce::MidiBuffer& buffer, int offset);

	friend class SynthThread;

	void initAudio(double sampleRate, double length);
	void initMIDI();

	const juce::String getAudioFileName() const;
	const juce::String getMIDIFileName() const;
	bool isAudioSaved() const;
	bool isMIDISaved() const;
	bool isAudioValid() const;
	bool isMIDIValid() const;

	double getSourceLength() const;

	/** Format, MetaData, BitDepth, Quality */
	using AudioFormat = std::tuple<juce::String, juce::StringPairArray, int, int>;
	const AudioFormat getAudioFormat() const;

	void linkInstr();
	void unlinkInstr();

	void invokeDataCallbacks() const;

	JUCE_DECLARE_WEAK_REFERENCEABLE(SeqSourceProcessor)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqSourceProcessor)
};
