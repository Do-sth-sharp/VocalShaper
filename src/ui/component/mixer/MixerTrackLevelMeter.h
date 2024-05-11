#pragma once

#include <JuceHeader.h>
#include "../../misc/LevelMeterHub.h"

class MixerTrackLevelMeter final
	: public juce::Component,
	public LevelMeterHub::Target,
	public juce::SettableTooltipClient {
public:
	MixerTrackLevelMeter();

	void updateLevelMeter() override;

	void paint(juce::Graphics& g) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;

	void update(int index);

private:
	int index = -1;
	juce::Array<float> values;
	bool mouseHovered = false;
	juce::Point<int> mousePos;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackLevelMeter)
};
