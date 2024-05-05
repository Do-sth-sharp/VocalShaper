#pragma once

#include <JuceHeader.h>
#include "../misc/LevelMeterHub.h"

class SeqTrackLevelMeter final
	: public juce::Component,
	public LevelMeterHub::Target,
	public juce::SettableTooltipClient {
public:
	SeqTrackLevelMeter();

	void updateLevelMeter() override;

	void paint(juce::Graphics& g) override;

	void update(int index);

private:
	int index = -1;
	juce::Array<float> values;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackLevelMeter)
};
