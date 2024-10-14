#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class EditorSwitchBarLookAndFeel : public MainLookAndFeel {
public:
	EditorSwitchBarLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorSwitchBarLookAndFeel)
};
