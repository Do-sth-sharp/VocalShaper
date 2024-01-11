#pragma once

#include <JuceHeader.h>

class ToolsLookAndFeel : public juce::LookAndFeel_V4 {
public:
	ToolsLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolsLookAndFeel)
};

