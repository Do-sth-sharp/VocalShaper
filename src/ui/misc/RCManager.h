#pragma once

#include <JuceHeader.h>

class RCManager final : private juce::DeletedAtShutdown {
public:
	RCManager() = default;
	~RCManager();

	void clear();
	juce::Image loadImage(const juce::File& file);

public:
	static RCManager* getInstance();
	static void releaseInstance();

private:
	static RCManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RCManager)
};
