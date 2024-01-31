#pragma once

#include <JuceHeader.h>

class MixerTrackComponent final : public juce::Component {
public:
	MixerTrackComponent();

	void paint(juce::Graphics& g) override;

	void update(int index);

	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

private:
	int index = -1;
	juce::Colour trackColor, nameColor;
	juce::String name;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackComponent)
};
