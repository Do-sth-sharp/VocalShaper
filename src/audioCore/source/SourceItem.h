#pragma once

#include <JuceHeader.h>
#include "SourceInternalContainer.h"

class SourceItem final {
public:
	using SourceType = SourceInternalContainer::SourceType;

	SourceItem() = delete;
	SourceItem(SourceType type);
	~SourceItem();

	void initAudio(int channelNum, double sampleRate, double length);
	void initMIDI();
	void setAudio(double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name);
	void setMIDI(const juce::MidiFile& data, const juce::String& name);
	void setAudio(const juce::String& name);
	void setMIDI(const juce::String& name);
	const std::tuple<double, juce::AudioSampleBuffer> getAudio() const;
	const juce::MidiMessageSequence getMIDI(int trackIndex) const;
	const juce::MidiFile getMIDIFile() const;

	void changed();
	void saved();
	bool isSaved() const;

	void prepareAudioPlay();
	void prepareMIDIPlay();
	void prepareAudioRecord(int channelNum);
	void prepareMIDIRecord();

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

	void forkIfNeed();

public:
	void readAudioData(juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	void readMIDIData(juce::MidiBuffer& buffer, double baseTime,
		double startTime, double endTime, int trackIndex) const;
	void writeAudioData(juce::AudioBuffer<float>& buffer,
		int offset, int trackChannelNum);
	void writeMIDIData(const juce::MidiBuffer& buffer,
		int offset, int trackIndex);

public:
	int getMIDINoteNum(int track) const;
	int getMIDISustainPedalNum(int track) const;
	int getMIDISostenutoPedalNum(int track) const;
	int getMIDISoftPedalNum(int track) const;
	int getMIDIPitchWheelNum(int track) const;
	int getMIDIAfterTouchNum(int track) const;
	int getMIDIChannelPressureNum(int track) const;
	const std::set<uint8_t> getMIDIControllerNumbers(int track) const;
	int getMIDIControllerNum(int track, uint8_t number) const;
	int getMIDIMiscNum(int track) const;

	const SourceMIDITemp::Note getMIDINote(int track, int index) const;
	const SourceMIDITemp::Pedal getMIDISustainPedal(int track, int index) const;
	const SourceMIDITemp::Pedal getMIDISostenutoPedal(int track, int index) const;
	const SourceMIDITemp::Pedal getMIDISoftPedal(int track, int index) const;
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

	ChangedCallback callback;

	void updateAudioResampler();

	void prepareAudioData(double length, int channelNum);
	void prepareMIDIData();

	void releaseContainer();
};
