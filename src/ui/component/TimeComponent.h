#pragma once

#include <JuceHeader.h>

class TimeComponent final : public juce::Component {
public:
	TimeComponent();

	void paint(juce::Graphics& g) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeComponent)
};
