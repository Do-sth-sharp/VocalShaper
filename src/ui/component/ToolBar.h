#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>

class ToolBar final : public flowUI::FlowComponent {
public:
	ToolBar();

	void resized() override;
	void paint(juce::Graphics& g) override;

private:
	std::unique_ptr<juce::DrawableButton> mainButton = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolBar)
};
