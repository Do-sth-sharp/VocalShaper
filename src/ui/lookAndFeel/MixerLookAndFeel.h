#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class MixerLookAndFeel : public MainLookAndFeel {
public:
	MixerLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerLookAndFeel)
};
