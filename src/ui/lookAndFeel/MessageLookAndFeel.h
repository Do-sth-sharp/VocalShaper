#pragma once

#include <JuceHeader.h>

class MessageLookAndFeel : public juce::LookAndFeel_V4 {
public:
	MessageLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageLookAndFeel)
};

