#pragma once

#include <JuceHeader.h>
#include "SeqTrackLookAndFeel.h"

class InstrNameLookAndFeel : public SeqTrackLookAndFeel {
public:
	InstrNameLookAndFeel();

	juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
	void drawButtonText(juce::Graphics& g, juce::TextButton& b,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;


private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrNameLookAndFeel)
};