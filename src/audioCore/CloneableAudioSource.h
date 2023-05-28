#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class CloneableAudioSource final : public CloneableSource {
public:
	CloneableAudioSource() = default;
	~CloneableAudioSource() override = default;

	void setSampleRate(double sampleRate);
	double getSampleRate() const;
	double getSourceSampleRate() const;

private:
	bool clone(const CloneableSource* src) override;
	bool load(const juce::File& file) override;
	bool save(const juce::File& file) const override;
	double getLength() const override;

private:
	juce::AudioSampleBuffer buffer;
	std::unique_ptr<juce::MemoryAudioSource> memorySource = nullptr;
	std::unique_ptr<juce::ResamplingAudioSource> source = nullptr;
	double sourceSampleRate = 0;
	double currentSampleRate = 0;

	int bitsPerSample = 32;
	juce::StringPairArray metadataValues;
	int qualityOptionIndex = 0;

	static juce::AudioFormat* findAudioFormat(const juce::File& file);
	static std::unique_ptr<juce::AudioFormatReader> createAudioReader(const juce::File& file);
	static std::unique_ptr<juce::AudioFormatWriter> createAudioWriter(const juce::File& file,
		double sampleRateToUse, const juce::AudioChannelSet& channelLayout,
		int bitsPerSample, const juce::StringPairArray& metadataValues, int qualityOptionIndex);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableAudioSource)
};