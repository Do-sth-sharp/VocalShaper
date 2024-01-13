#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class ToolsLookAndFeel : public MainLookAndFeel {
public:
	ToolsLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolsLookAndFeel)
};

