#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>

class MixerView final : public flowUI::FlowComponent {
public:
	MixerView();

	void paint(juce::Graphics& g) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerView)
};
