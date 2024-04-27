#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class RecButtonLookAndFeel : public MainLookAndFeel {
public:
	RecButtonLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecButtonLookAndFeel)
};
