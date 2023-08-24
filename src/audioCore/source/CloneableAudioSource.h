#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class CloneableAudioSource final : public CloneableSource {
public:
	CloneableAudioSource() = default;
	~CloneableAudioSource() override = default;

	double getSourceSampleRate() const;

	void readData(juce::AudioBuffer<float>& buffer, double bufferDeviation,
		double dataDeviation, double length) const;
	int getChannelNum() const;

private:
	bool clone(const CloneableSource* src) override;
	bool load(const juce::File& file) override;
	bool save(const juce::File& file) const override;
	double getLength() const override;
	void sampleRateChanged() override;

private:
	juce::AudioSampleBuffer buffer;
	juce::CriticalSection lock;
	std::unique_ptr<juce::MemoryAudioSource> memorySource = nullptr;
	std::unique_ptr<juce::ResamplingAudioSource> source = nullptr;
	double sourceSampleRate = 0;

	int bitsPerSample = 32;
	juce::StringPairArray metadataValues;
	int qualityOptionIndex = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableAudioSource)
};