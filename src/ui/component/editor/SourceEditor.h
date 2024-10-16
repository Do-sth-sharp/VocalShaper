#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "SourceSwitchBar.h"

class SourceEditor final : public flowUI::FlowComponent {
public:
	SourceEditor();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();
	void setTrack(int trackIndex);
	void update(int trackIndex);
	void update(uint64_t audioRef, uint64_t midiRef);

private:
	std::unique_ptr<SourceSwitchBar> switchBar = nullptr;
	SourceSwitchBar::SwitchState switchState = SourceSwitchBar::SwitchState::Off;

	int trackIndex = -1;
	uint64_t audioRef = 0, midiRef = 0;

	juce::String emptyStr;

	void switchEditor(SourceSwitchBar::SwitchState state);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceEditor)
};
