#pragma once

#include <JuceHeader.h>

class RecordTemp final : private juce::DeletedAtShutdown {
public:
	RecordTemp();

	void setInputSampleRate(double sampleRate);
	void setInputChannelNum(int channels);
	void recordData(double timeSec,
		const juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiMessages);

	void setRecordMIDI(bool recordMIDI);
	void setRecordAudio(bool recordAudio);
	bool isRecordMIDI() const;
	bool isRecordAudio() const;

	void clearAll();
	void clearMIDI();
	void clearAudio();

	double getSampleRate() const;
	double getStartTime() const;
	const juce::MidiMessageSequence getMIDIData() const;
	const juce::AudioSampleBuffer getAudioData() const;

	/** Sample Rate, Start Time, MIDI Data, Audio Data */
	using DataPacked = std::tuple<double, double, const juce::MidiMessageSequence, const juce::AudioSampleBuffer>;
	const DataPacked getDataPacked() const;

private:
	juce::CriticalSection lock;
	double sampleRate = 0;

	juce::MidiMessageSequence midiBuffer;
	juce::AudioSampleBuffer audioBuffer;
	double startTime = -1;

	bool recordMIDI = true, recordAudio = true;

	bool tryToEnsureAudioBufferSamplesAllocated(uint64_t sampleNum);

public:
	static RecordTemp* getInstance();
	static void releaseInstance();

private:
	static RecordTemp* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordTemp)
};
