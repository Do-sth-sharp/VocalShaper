#pragma once

#include <JuceHeader.h>

class AudioExtractorJob;

class AudioExtractor final : private juce::DeletedAtShutdown {
public:
	AudioExtractor() = default;

	using Result = juce::Array<juce::MemoryBlock>;
	using Callback = std::function<void(const Result&)>;

	void extractAsync(const void* ticket,
		const juce::AudioSampleBuffer& data, uint64_t pointNum,
		const Callback& callback);

private:
	struct DataTemp {
		juce::AudioSampleBuffer data;
		std::shared_ptr<juce::ThreadPoolJob> job;
	};
	std::map<const void*, DataTemp> templist;

	friend class AudioExtractorJob;
	void destoryTicket(const void* ticket);

public:
	static AudioExtractor* getInstance();
	static void releaseInstance();

private:
	static AudioExtractor* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioExtractor)
};
