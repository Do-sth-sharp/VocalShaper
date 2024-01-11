#pragma once

#include <JuceHeader.h>

class ToolBarLookAndFeel : public juce::LookAndFeel_V4 {
public:
	ToolBarLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolBarLookAndFeel)
};
