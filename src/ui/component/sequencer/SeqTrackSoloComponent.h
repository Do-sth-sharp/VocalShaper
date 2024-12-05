#pragma once

#include <JuceHeader.h>

class SeqTrackSoloComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	SeqTrackSoloComponent();

	void paint(juce::Graphics& g) override;

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	void update(int index);

private:
	int index = -1;
	bool solo = false;

	void changeSolo();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackSoloComponent)
};
