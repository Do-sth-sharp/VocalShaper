#pragma once

#include <JuceHeader.h>

class AudioSampleTemp final : private juce::DeletedAtShutdown {
public:
	AudioSampleTemp() = default;

	void setAudio(int index, const juce::AudioSampleBuffer& data, double sampleRate);
	const juce::AudioSampleBuffer* getAudioData(int index) const;
	double getSampleRate(int index) const;

	void clear();

private:
	struct TempData {
		double sampleRate;
		juce::AudioSampleBuffer audioData;
	};
	juce::OwnedArray<TempData> list;
	juce::ReadWriteLock lock;

public:
	static AudioSampleTemp* getInstance();
	static void releaseInstance();

private:
	static AudioSampleTemp* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSampleTemp)
};
