#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class SystemStatusLookAndFeel : public MainLookAndFeel {
public:
	SystemStatusLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SystemStatusLookAndFeel)
};
