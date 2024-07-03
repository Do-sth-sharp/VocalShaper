#pragma once

#include <JuceHeader.h>

class SourceManager final : private juce::DeletedAtShutdown {
public:
	SourceManager() = default;

public:
	static SourceManager* getInstance();
	static void releaseInstance();

private:
	static SourceManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceManager)
};
