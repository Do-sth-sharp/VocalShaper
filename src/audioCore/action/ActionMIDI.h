#pragma once

#include "ActionBase.h"
#include "../quickAPI/QuickGet.h"

class ActionMIDIAddNote final : public ActionUndoableBase {
public:
	ActionMIDIAddNote() = delete;
	ActionMIDIAddNote(
		uint64_t ref, int track, double startTime, double endTime, uint8_t channel,
		uint8_t pitch, uint8_t vel, const juce::String& lyrics = "");

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "MIDI Add Note";
	};
	ActionType getActionType() const override { return ActionType::ActionMIDIAddNote; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const uint64_t ref;
	const int track;
	const double startTime;
	const double endTime;
	const uint8_t channel;
	const uint8_t pitch;
	const uint8_t vel;
	const juce::String lyrics;

	int index = -1;

	JUCE_LEAK_DETECTOR(ActionMIDIAddNote)
};
