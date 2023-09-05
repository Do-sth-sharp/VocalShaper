#pragma once

#include <JuceHeader.h>

class PlayWatcher final : public juce::Timer,
	private juce::DeletedAtShutdown {
public:
	PlayWatcher() = default;
	~PlayWatcher() override;

public:
	void timerCallback() override;

public:
	static PlayWatcher* getInstance();
	static void releaseInstance();

private:
	static PlayWatcher* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayWatcher)
};
