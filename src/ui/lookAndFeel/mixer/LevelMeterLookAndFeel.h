#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class LevelMeterLookAndFeel : public MainLookAndFeel {
public:
	LevelMeterLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterLookAndFeel)
};