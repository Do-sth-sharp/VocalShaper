#pragma once

#include <JuceHeader.h>

class EffectComponent final : public juce::Component {
public:
	EffectComponent();

	void paint(juce::Graphics& g) override;

	void update(int track, int index);

private:
	int track = -1, index = -1;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectComponent)
};
