#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class ScrollerLookAndFeel : public MainLookAndFeel {
public:
	ScrollerLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollerLookAndFeel)
};