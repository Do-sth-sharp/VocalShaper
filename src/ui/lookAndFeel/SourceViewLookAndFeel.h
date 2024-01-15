#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class SourceViewLookAndFeel : public MainLookAndFeel {
public:
	SourceViewLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceViewLookAndFeel)
};
