#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "SourceSwitchBar.h"

class SourceEditor final : public flowUI::FlowComponent {
public:
	SourceEditor();

	void resized() override;
	void paint(juce::Graphics& g) override;

private:
	std::unique_ptr<SourceSwitchBar> switchBar = nullptr;

	void switchEditor(SourceSwitchBar::SwitchState state);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceEditor)
};
