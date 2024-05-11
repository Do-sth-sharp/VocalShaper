#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class EffectLookAndFeel : public MainLookAndFeel {
public:
	EffectLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectLookAndFeel)
};
