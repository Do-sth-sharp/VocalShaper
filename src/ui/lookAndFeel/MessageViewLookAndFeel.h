#pragma once

#include <JuceHeader.h>

class MessageViewLookAndFeel : public juce::LookAndFeel_V4 {
public:
	MessageViewLookAndFeel();

	void drawCallOutBoxBackground(
		juce::CallOutBox&, juce::Graphics&, const juce::Path&, juce::Image&) override;
	int getCallOutBoxBorderSize(const juce::CallOutBox&) override;
	float getCallOutBoxCornerSize(const juce::CallOutBox&) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageViewLookAndFeel)
};
