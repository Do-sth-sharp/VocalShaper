#pragma once

#include <JuceHeader.h>

class SystemStatusLookAndFeel : public juce::LookAndFeel_V4 {
public:
	SystemStatusLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SystemStatusLookAndFeel)
};
