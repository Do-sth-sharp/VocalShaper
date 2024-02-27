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

	int addSeq(const SourceList::SeqBlock& block);
	void removeSeq(int index);
	int getSeqNum() const;
	const SourceList::SeqBlock getSeq(int index) const;

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

	double getSourceLength() const;
	double getMIDILength() const;
	double getAudioLength() const;

	void initAudio(double sampleRate, int channelNum, int sampleNum);
	void initMIDI();
	void setAudio(double sampleRate, const juce::AudioSampleBuffer& data);
	void setMIDI(const juce::MidiFile& data);
	const std::tuple<double, juce::AudioSampleBuffer> getAudio() const;
	const juce::MidiFile getMIDI() const;

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
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	const juce::AudioChannelSet audioChannels;

	SourceList srcs;
	std::set<std::tuple<int, int>> activeNoteSet;
	std::atomic_bool noteCloseFlag = false;

	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioProcessorGraph::Node::Ptr instr = nullptr;

	juce::String trackName;
	juce::Colour trackColor;

	std::unique_ptr<juce::MidiFile> midiData = nullptr;
	std::unique_ptr<juce::AudioSampleBuffer> audioData = nullptr;
	std::unique_ptr<juce::MemoryAudioSource> memSource = nullptr;
	std::unique_ptr<juce::ResamplingAudioSource> resampleSource = nullptr;
	double audioSampleRate = 0;

	void prepareAudioPlay(double sampleRate, int maximumExpectedSamplesPerBlock);
	void prepareMIDIPlay(double sampleRate, int maximumExpectedSamplesPerBlock);

	void readAudioData(juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	void readMIDIData(juce::MidiBuffer& buffer, int baseTime,
		int startTime, int endTime) const;

	void updateAudioResampler();

	JUCE_DECLARE_WEAK_REFERENCEABLE(SeqSourceProcessor)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqSourceProcessor)
};
