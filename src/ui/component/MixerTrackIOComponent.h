#pragma once

#include <JuceHeader.h>

class MixerTrackIOComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	MixerTrackIOComponent() = delete;
	MixerTrackIOComponent(bool isInput, bool isMidi);

	void paint(juce::Graphics& g) override;

	void update(int index);

private:
	const bool isInput, isMidi;
	int index = -1;
	bool linked = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackIOComponent)
};
