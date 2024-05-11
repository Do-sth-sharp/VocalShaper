#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class MuteButtonLookAndFeel : public MainLookAndFeel {
public:
	MuteButtonLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MuteButtonLookAndFeel)
};
