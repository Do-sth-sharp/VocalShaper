#pragma once

#include <JuceHeader.h>
#include "MainLookAndFeel.h"

class SourceViewLookAndFeel : public MainLookAndFeel {
public:
	SourceViewLookAndFeel();

	juce::Font getTextButtonFont(juce::TextButton& b, int) override;
	void drawButtonBackground(juce::Graphics&, juce::Button&,
		const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted,
		bool shouldDrawButtonAsDown) override;
	void drawButtonText(juce::Graphics&, juce::TextButton&,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceViewLookAndFeel)
};
