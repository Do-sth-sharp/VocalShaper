#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class TimeLookAndFeel : public MainLookAndFeel {
public:
	TimeLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeLookAndFeel)
};
