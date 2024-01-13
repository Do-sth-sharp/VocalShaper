#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class PluginViewLookAndFeel : public MainLookAndFeel {
public:
	PluginViewLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginViewLookAndFeel)
};
