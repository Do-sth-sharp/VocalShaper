#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class SeqLookAndFeel : public MainLookAndFeel {
public:
	SeqLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqLookAndFeel)
};
