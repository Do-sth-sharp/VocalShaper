#pragma once

#include <JuceHeader.h>

class CloneableSourceManager final : public juce::DeletedAtShutdown {
public:
	CloneableSourceManager() = default;
	~CloneableSourceManager() override = default;

public:
	static CloneableSourceManager* getInstance();
	static void releaseInstance();

private:
	static CloneableSourceManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableSourceManager)
};
