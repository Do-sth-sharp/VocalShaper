#pragma once

#include <JuceHeader.h>

class MixerTrackSoloComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	MixerTrackSoloComponent();

	void paint(juce::Graphics& g) override;

	void mouseUp(const juce::MouseEvent& event) override;

	void updateIndex(int type, int index);
	void update();

private:
	int type = -1, index = -1;
	bool solo = false;

	void changeSolo();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackSoloComponent)
};
