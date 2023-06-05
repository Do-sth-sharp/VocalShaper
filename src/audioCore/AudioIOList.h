#pragma once

#include <JuceHeader.h>

class AudioIOList final : public juce::Thread,
	private juce::DeletedAtShutdown {
public:
	AudioIOList();

private:
	void run() override {};

public:
	static AudioIOList* getInstance();
	static void releaseInstance();

private:
	static AudioIOList* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioIOList)
};