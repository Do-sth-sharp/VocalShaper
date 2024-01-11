#pragma once

#include <JuceHeader.h>

class ControllerLookAndFeel : public juce::LookAndFeel_V4 {
public:
	ControllerLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerLookAndFeel)
};
