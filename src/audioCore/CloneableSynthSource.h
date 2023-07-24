#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class CloneableSynthSource final : public CloneableSource {
public:
	CloneableSynthSource() = default;
	~CloneableSynthSource() override = default;

	int getTrackNum() const;

	double getSourceSampleRate() const;
	void readData(juce::AudioBuffer<float>& buffer, double bufferDeviation,
		double dataDeviation, double length) const;
	int getChannelNum() const;

	void synth();

private:
	bool clone(const CloneableSource* src) override;
	bool load(const juce::File& file) override;
	bool save(const juce::File& file) const override;
	double getLength() const override;
	void sampleRateChanged() override;

private:
	juce::MidiFile buffer;
	juce::ReadWriteLock lock;

	juce::AudioSampleBuffer audioBuffer;
	juce::CriticalSection audioLock;
	std::unique_ptr<juce::MemoryAudioSource> memorySource = nullptr;
	std::unique_ptr<juce::ResamplingAudioSource> source = nullptr;
	double sourceSampleRate = 0;

	double tailTime = 2;
	const int audioChannels = 1;

	static void convertSecondsToTicks(juce::MidiFile& file);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableSynthSource)
};