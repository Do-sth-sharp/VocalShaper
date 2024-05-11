#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class SeqTrackNameLookAndFeel : public MainLookAndFeel {
public:
	SeqTrackNameLookAndFeel();

	void drawButtonBackground(juce::Graphics& g, juce::Button& b,
		const juce::Colour& backgroundColour,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
	void drawButtonText(juce::Graphics& g, juce::TextButton& b,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;


private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackNameLookAndFeel)
};
