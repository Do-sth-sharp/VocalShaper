#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class MessageViewLookAndFeel : public MainLookAndFeel {
public:
	MessageViewLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageViewLookAndFeel)
};
