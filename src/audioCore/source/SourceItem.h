#pragma once

#include <JuceHeader.h>

class SourceItem final {
public:
	enum class SourceType {
		Undefined, MIDI, Audio
	};

	SourceItem() = delete;
	SourceItem(SourceType type);

	void initAudio(int channelNum, double sampleRate, double length);
	void initMIDI();
	void setAudio(double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name);
	void setMIDI(const juce::MidiFile& data, const juce::String& name);
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

public:
	void readAudioData(juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	void readMIDIData(juce::MidiBuffer& buffer, double baseTime,
		double startTime, double endTime, int trackIndex) const;
	void writeAudioData(juce::AudioBuffer<float>& buffer,
		int offset, int trackChannelNum);
	void writeMIDIData(const juce::MidiBuffer& buffer,
		int offset, int trackIndex);

private:
	const SourceType type;
	std::unique_ptr<juce::MidiFile> midiData = nullptr;
	std::unique_ptr<juce::AudioSampleBuffer> audioData = nullptr;
	std::unique_ptr<juce::MemoryAudioSource> memSource = nullptr;
	std::unique_ptr<juce::ResamplingAudioSource> resampleSource = nullptr;
	double audioSampleRate = 0;
	juce::String fileName;
	std::atomic_bool savedFlag = true;

	const double recordInitLength = 30;
	juce::AudioSampleBuffer recordBuffer, recordBufferTemp;

	double playSampleRate = 0;
	int blockSize = 0;

	void updateAudioResampler();

	void prepareAudioData(double length, int channelNum);
	void prepareMIDIData();
};
