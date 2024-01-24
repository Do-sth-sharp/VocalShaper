#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class PluginEditorLookAndFeel: public MainLookAndFeel {
public:
	PluginEditorLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorLookAndFeel)
};