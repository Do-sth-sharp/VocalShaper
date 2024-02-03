#pragma once

#include <JuceHeader.h>
#include "SideChainComponent.h"

class MixerTrackComponent final : public juce::Component {
public:
	MixerTrackComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;

	void update(int index);

	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

private:
	int index = -1;
	juce::Colour trackColor, nameColor;
	juce::String name;

	std::unique_ptr<SideChainComponent> sideChain = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackComponent)
};
