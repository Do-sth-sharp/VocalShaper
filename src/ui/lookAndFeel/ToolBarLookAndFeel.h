#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class ToolBarLookAndFeel : public MainLookAndFeel {
public:
	ToolBarLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolBarLookAndFeel)
};
