#pragma once

#include <JuceHeader.h>

class ToolComponent final : public juce::Component {
public:
	ToolComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;

private:
	juce::OwnedArray<juce::DrawableButton> buttons;
	juce::OwnedArray<juce::Drawable> icons;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolComponent)
};
