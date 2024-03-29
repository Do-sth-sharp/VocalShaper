﻿#pragma once

#include <JuceHeader.h>

#include "SourceList.h"
#include "PluginDecorator.h"
#include "../project/Serializable.h"

class SeqSourceProcessor final : public juce::AudioProcessorGraph,
	public Serializable {
public:
	SeqSourceProcessor() = delete;
	SeqSourceProcessor(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());

	void updateIndex(int index);

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
	void setInstrOffline(bool offline);
	bool getInstrOffline() const;

	bool isSynthRunning() const;
	void startSynth();

	double getSourceLength() const;
	double getMIDILength() const;
	double getAudioLength() const;

	void initAudio(double sampleRate, double length);
	void initMIDI();
	void setAudio(double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name);
	void setMIDI(const juce::MidiFile& data, const juce::String& name);
	const std::tuple<double, juce::AudioSampleBuffer> getAudio() const;
	const juce::MidiFile getMIDI() const;
	void saveAudio(const juce::String& path = "") const;
	void saveMIDI(const juce::String& path = "") const;
	void loadAudio(const juce::String& path);
	void loadMIDI(const juce::String& path, bool getTempo = false);
	const juce::String getAudioFileName() const;
	const juce::String getMIDIFileName() const;
	void audioChanged();
	void midiChanged();
	void audioSaved();
	void midiSaved();
	bool isAudioSaved() const;
	bool isMIDISaved() const;

	void setRecording(bool recording);
	bool getRecording() const;

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
	int index = -1;

	const juce::AudioChannelSet audioChannels;

	SourceList srcs;
	std::set<std::tuple<int, int>> activeNoteSet;
	std::atomic_bool noteCloseFlag = false;

	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioProcessorGraph::Node::Ptr instr = nullptr;
	std::atomic_bool instrOffline = false;
	std::unique_ptr<juce::Thread> synthThread = nullptr;

	juce::String trackName;
	juce::Colour trackColor;

	std::unique_ptr<juce::MidiFile> midiData = nullptr;
	std::unique_ptr<juce::AudioSampleBuffer> audioData = nullptr;
	std::unique_ptr<juce::MemoryAudioSource> memSource = nullptr;
	std::unique_ptr<juce::ResamplingAudioSource> resampleSource = nullptr;
	double audioSampleRate = 0;
	juce::String audioName, midiName;
	std::atomic_bool audioSavedFlag = true, midiSavedFlag = true;

	std::atomic_bool recordingFlag = false;
	const double recordInitLength = 30;
	juce::AudioSampleBuffer recordBuffer, recordBufferTemp;

	void prepareAudioPlay(double sampleRate, int maximumExpectedSamplesPerBlock);
	void prepareMIDIPlay(double sampleRate, int maximumExpectedSamplesPerBlock);

	friend class SourceRecordProcessor;
	void readAudioData(juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	void readMIDIData(juce::MidiBuffer& buffer, int baseTime,
		int startTime, int endTime) const;
	void writeAudioData(juce::AudioBuffer<float>& buffer, int offset);
	void writeMIDIData(const juce::MidiBuffer& buffer, int offset);

	void updateAudioResampler();
	void prepareRecord();
	void prepareAudioRecord();
	void prepareMIDIRecord();

	friend class SynthThread;
	void prepareAudioData(double length);
	void prepareMIDIData();

	void linkInstr();
	void unlinkInstr();

	JUCE_DECLARE_WEAK_REFERENCEABLE(SeqSourceProcessor)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqSourceProcessor)
};
