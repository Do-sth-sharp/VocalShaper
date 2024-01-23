#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class InstrViewLookAndFeel : public MainLookAndFeel {
public:
	InstrViewLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrViewLookAndFeel)
};