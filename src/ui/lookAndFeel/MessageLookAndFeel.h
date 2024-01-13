#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class MessageLookAndFeel : public MainLookAndFeel {
public:
	MessageLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageLookAndFeel)
};

