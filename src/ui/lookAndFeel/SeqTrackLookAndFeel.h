#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class SeqTrackLookAndFeel : public MainLookAndFeel {
public:
	SeqTrackLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackLookAndFeel)
};
