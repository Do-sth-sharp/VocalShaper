#pragma once

#include <JuceHeader.h>

class MixerTrackComponent final : public juce::Component {
public:
	MixerTrackComponent();

	void paint(juce::Graphics& g) override;

	void update(int index);

private:
	int index = -1;
	juce::Colour trackColor, nameColor;
	juce::String name;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackComponent)
};
