#pragma once

#include <JuceHeader.h>
#include "SourceInternalContainer.h"

class SourceItem final {
public:
	using SourceType = SourceInternalContainer::SourceType;

	SourceItem() = delete;
	SourceItem(SourceType type);
	~SourceItem();

	void initAudio(
		const juce::String& name,
		int channelNum, double sampleRate, double length);
	void initMIDI(const juce::String& name);
	void setAudio(double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name);
	void setMIDI(const juce::MidiFile& data, const juce::String& name);
	void setAudio(const juce::String& name);
	void setMIDI(const juce::String& name);
	const std::tuple<double, juce::AudioSampleBuffer> getAudio() const;
	const juce::MidiMessageSequence makeMIDITrack(int trackIndex) const;
	const juce::MidiFile makeMIDIFile() const;

	using AudioWriteType = SourceInternalContainer::AudioWriteType;
	using MIDIWriteType = SourceInternalContainer::MIDIWriteType;
	void writeAudio(AudioWriteType type, const juce::AudioSampleBuffer& buffer,
		double startTime, double length, double sampleRate);
	void writeMIDI(MIDIWriteType type, const juce::MidiMessageSequence& sequence,
		double startTime, double length, int track);

	void changed();
	void saved();
	bool isSaved() const;

	void prepareAudioPlay();
	void prepareMIDIPlay();

	void setSampleRate(int blockSize, double sampleRate);

	SourceType getType() const;
	const juce::String getFileName() const;
	bool midiValid() const;
	bool audioValid() const;
	int getMIDITrackNum() const;
	double getMIDILength() const;
	double getAudioLength() const;

	using ChangedCallback = std::function<void(void)>;
	void setCallback(const ChangedCallback& callback);
	void invokeCallback() const;

	using AudioFormat = SourceInternalContainer::AudioFormat;
	void setAudioFormat(const AudioFormat& format);
	const AudioFormat getAudioFormat() const;
	double getAudioSampleRate() const;

	void forkIfNeed();

public:
	void readAudioData(juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	void readMIDIData(juce::MidiBuffer& buffer, double baseTime,
		double startTime, double endTime, int trackIndex) const;

public:
	int getMIDINoteNum(int track) const;
	int getMIDIPitchWheelNum(int track) const;
	int getMIDIAfterTouchNum(int track) const;
	int getMIDIChannelPressureNum(int track) const;
	const std::set<uint8_t> getMIDIControllerNumbers(int track) const;
	int getMIDIControllerNum(int track, uint8_t number) const;
	int getMIDIMiscNum(int track) const;

	const SourceMIDITemp::Note getMIDINote(int track, int index) const;
	const SourceMIDITemp::IntParam getMIDIPitchWheel(int track, int index) const;
	const SourceMIDITemp::AfterTouch getMIDIAfterTouch(int track, int index) const;
	const SourceMIDITemp::IntParam getMIDIChannelPressure(int track, int index) const;
	const SourceMIDITemp::Controller getMIDIController(int track, uint8_t number, int index) const;
	const SourceMIDITemp::Misc getMIDIMisc(int track, int index) const;

private:
	const SourceType type;
	std::shared_ptr<SourceInternalContainer> container = nullptr;

	std::unique_ptr<juce::MemoryAudioSource> memSource = nullptr;
	std::unique_ptr<juce::ResamplingAudioSource> resampleSource = nullptr;

	const double recordInitLength = 30;
	juce::AudioSampleBuffer recordBuffer, recordBufferTemp;

	double playSampleRate = 0;
	int blockSize = 0;

	mutable int playbackMIDIIndexTemp = -1;

	ChangedCallback callback;

	void updateAudioResampler();

	void releaseContainer();
};
