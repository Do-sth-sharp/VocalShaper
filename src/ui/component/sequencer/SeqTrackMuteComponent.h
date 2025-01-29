#pragma once

#include <JuceHeader.h>

class SeqTrackMuteComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	SeqTrackMuteComponent();

	void paint(juce::Graphics& g) override;

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	void updateIndex(int index);
	void update();

private:
	int index = -1;
	bool mute = false;
	bool equivalentMute = false;

	void changeMute();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackMuteComponent)
};
