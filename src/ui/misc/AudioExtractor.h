#pragma once

#include <JuceHeader.h>

class AudioExtractor final {
	AudioExtractor() = delete;

public:
	using AudioData = std::tuple<double, juce::AudioSampleBuffer>;
	using Result = juce::Array<juce::MemoryBlock>;
	using Callback = std::function<void(const Result&)>;

	static void extractAsync(
		const AudioData& data, uint64_t pointNum,
		const Callback& callback);

private:
	static void extractInternal(
		const juce::AudioSampleBuffer& data, uint64_t pointNum,
		const Callback& callback);
};
