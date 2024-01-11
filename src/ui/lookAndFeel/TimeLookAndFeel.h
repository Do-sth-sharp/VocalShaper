#pragma once

#include <JuceHeader.h>

class TimeLookAndFeel : public juce::LookAndFeel_V4 {
public:
	TimeLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeLookAndFeel)
};
