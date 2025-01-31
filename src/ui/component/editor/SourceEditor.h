#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "SourceSwitchBar.h"
#include "MIDISourceEditor.h"
#include "AudioSourceEditor.h"

class SourceEditor final : public flowUI::FlowComponent {
public:
	SourceEditor();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update();
	void setTrack(int trackIndex);
	void update(int trackIndex);
	void update(uint64_t audioRef, uint64_t midiRef);
	void updateTempo();
	void updateBlocks(int trackIndex);
	void updateData(int trackIndex);

	int getCurrentIndex() const;

private:
	std::unique_ptr<SourceSwitchBar> switchBar = nullptr;
	SourceSwitchBar::SwitchState switchState = SourceSwitchBar::SwitchState::Off;

	std::unique_ptr<MIDISourceEditor> midiEditor = nullptr;
	std::unique_ptr<AudioSourceEditor> audioEditor = nullptr;

	int trackIndex = -1;
	uint64_t audioRef = 0, midiRef = 0;

	juce::String emptyStr;

	std::unique_ptr<juce::ChangeListener> editingTrackListener = nullptr;

	void switchEditor(SourceSwitchBar::SwitchState state);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceEditor)
};
