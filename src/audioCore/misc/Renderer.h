#pragma once

#include <JuceHeader.h>

class Renderer final : private juce::DeletedAtShutdown {
public:
	Renderer() = default;

public:
	static Renderer* getInstance();
	static void releaseInstance();

private:
	static Renderer* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Renderer)
};
