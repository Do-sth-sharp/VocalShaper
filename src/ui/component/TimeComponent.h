#pragma once

#include <JuceHeader.h>

class TimeComponent final : public juce::Component {
public:
	TimeComponent();

	void paint(juce::Graphics& g) override;

private:
	double timeInSec = 0, timeInBeat = 0;

	static uint8_t convertBits(uint8_t num);
	static void paintNum(
		juce::Graphics& g, const juce::Rectangle<int>& area,
		float lineThickness, float splitThickness, uint8_t num);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeComponent)
};
