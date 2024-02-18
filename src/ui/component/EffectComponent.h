#pragma once

#include <JuceHeader.h>

class EffectComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	EffectComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int track, int index);

	void mouseUp(const juce::MouseEvent& event) override;

private:
	int track = -1, index = -1;
	juce::String name;
	bool editorOpened = false;

	std::unique_ptr<juce::Drawable> bypassIcon = nullptr;
	std::unique_ptr<juce::Drawable> bypassIconOn = nullptr;
	std::unique_ptr<juce::DrawableButton> bypassButton = nullptr;

	void bypass();
	void editorShow();

	juce::String createToolTip() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectComponent)
};
