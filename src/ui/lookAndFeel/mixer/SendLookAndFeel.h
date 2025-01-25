#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class SendLookAndFeel : public MainLookAndFeel {
public:
	SendLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendLookAndFeel)
};
