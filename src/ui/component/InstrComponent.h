#pragma once

#include <JuceHeader.h>

class InstrComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	InstrComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int index);

private:
	int index = -1;
	juce::String name;

	std::unique_ptr<juce::Drawable> bypassIcon = nullptr;
	std::unique_ptr<juce::Drawable> bypassIconOn = nullptr;
	std::unique_ptr<juce::DrawableButton> bypassButton = nullptr;

	void bypass();

	juce::String createToolTip() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrComponent)
};
