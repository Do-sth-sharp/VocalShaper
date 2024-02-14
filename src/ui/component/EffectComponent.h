#pragma once

#include <JuceHeader.h>

class EffectComponent final : public juce::Component {
public:
	EffectComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int track, int index);

private:
	int track = -1, index = -1;

	std::unique_ptr<juce::Drawable> bypassIcon = nullptr;
	std::unique_ptr<juce::Drawable> bypassIconOn = nullptr;
	std::unique_ptr<juce::DrawableButton> bypassButton = nullptr;

	void bypass();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectComponent)
};
