#pragma once

#include <JuceHeader.h>

class MixerTrackMuteComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	MixerTrackMuteComponent();

	void update(int index);

private:
	int index = -1;
	bool mute = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackMuteComponent)
};
