#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class SoloButtonLookAndFeel : public MainLookAndFeel {
public:
	SoloButtonLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloButtonLookAndFeel)
};
