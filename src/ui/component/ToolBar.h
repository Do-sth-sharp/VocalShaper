#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>

class ToolBar final : public flowUI::FlowComponent {
public:
	ToolBar();

	void paint(juce::Graphics& g) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolBar)
};
