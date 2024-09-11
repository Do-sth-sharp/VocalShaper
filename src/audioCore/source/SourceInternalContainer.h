#pragma once

#include <JuceHeader.h>
#include "SourceMIDITemp.h"

class SourceInternalContainer {
public:
	enum class SourceType {
		Undefined, MIDI, Audio
	};

	SourceInternalContainer(const SourceType type, const juce::String& name);
	SourceInternalContainer(const SourceInternalContainer& other);

	SourceType getType() const;
	const juce::String getName() const;

	juce::MidiFile* getMidiData() const;
	juce::AudioSampleBuffer* getAudioData() const;
	double getAudioSampleRate() const;

	void changed();
	void saved();
	bool isSaved() const;

	const juce::String getFormat() const;
	const juce::StringPairArray getMetaData() const;
	int getBitsPerSample() const;
	int getQuality() const;

	void initMidiData();
	void initAudioData(int channelNum, double sampleRate, double length);

	void setMIDI(const juce::MidiFile& data);
	void setAudio(double sampleRate, const juce::AudioSampleBuffer& data);

	using AudioFormat = std::tuple<juce::String, juce::StringPairArray, int, int>;
	void setAudioFormat(const AudioFormat& format);
	const AudioFormat getAudioFormat() const;

	bool isForked() const;

public:
	int getTrackNum() const;

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
	const juce::String name;

	const bool forked = false;

	std::unique_ptr<SourceMIDITemp> midiData = nullptr;
	std::unique_ptr<juce::AudioSampleBuffer> audioData = nullptr;
	double audioSampleRate = 0;
	std::atomic_bool savedFlag = true;

	juce::String format;
	juce::StringPairArray metaData;
	int bitsPerSample = 0;
	int quality = 0;

	void initAudioFormat();

	static const juce::String getForkName(const juce::String& name);

	JUCE_LEAK_DETECTOR(SourceInternalContainer)
};
