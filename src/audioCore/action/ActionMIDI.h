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

class ActionMIDISetNoteTime final : public ActionUndoableBase {
public:
	ActionMIDISetNoteTime() = delete;
	ActionMIDISetNoteTime(
		uint64_t ref, int track, int index,
		double startTime, double endTime);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "MIDI Set Note Time";
	};
	ActionType getActionType() const override { return ActionType::ActionMIDISetNoteTime; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const uint64_t ref;
	const int track, index;
	const double startTime;
	const double endTime;

	int oldEndIndex = -1;
	int newIndex = -1;
	double oldStartTime = 0, oldEndTime = 0;

	JUCE_LEAK_DETECTOR(ActionMIDISetNoteTime)
};

class ActionMIDISetNoteChannel final : public ActionUndoableBase {
public:
	ActionMIDISetNoteChannel() = delete;
	ActionMIDISetNoteChannel(
		uint64_t ref, int track, int index,
		uint8_t channel);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "MIDI Set Note Channel";
	};
	ActionType getActionType() const override { return ActionType::ActionMIDISetNoteChannel; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const uint64_t ref;
	const int track, index;
	const uint8_t channel;

	uint8_t oldChannel = 0;

	JUCE_LEAK_DETECTOR(ActionMIDISetNoteChannel)
};

class ActionMIDISetNotePitch final : public ActionUndoableBase {
public:
	ActionMIDISetNotePitch() = delete;
	ActionMIDISetNotePitch(
		uint64_t ref, int track, int index,
		uint8_t pitch);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "MIDI Set Note Pitch";
	};
	ActionType getActionType() const override { return ActionType::ActionMIDISetNotePitch; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const uint64_t ref;
	const int track, index;
	const uint8_t pitch;

	uint8_t oldPitch = 0;

	JUCE_LEAK_DETECTOR(ActionMIDISetNotePitch)
};

class ActionMIDISetNoteVelocity final : public ActionUndoableBase {
public:
	ActionMIDISetNoteVelocity() = delete;
	ActionMIDISetNoteVelocity(
		uint64_t ref, int track, int index,
		uint8_t velocity);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "MIDI Set Note Velocity";
	};
	ActionType getActionType() const override { return ActionType::ActionMIDISetNoteVelocity; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const uint64_t ref;
	const int track, index;
	const uint8_t velocity;

	uint8_t oldVelocity = 0;

	JUCE_LEAK_DETECTOR(ActionMIDISetNoteVelocity)
};

class ActionMIDISetNoteLyrics final : public ActionUndoableBase {
public:
	ActionMIDISetNoteLyrics() = delete;
	ActionMIDISetNoteLyrics(
		uint64_t ref, int track, int index,
		const juce::String& lyrics);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "MIDI Set Note Lyrics";
	};
	ActionType getActionType() const override { return ActionType::ActionMIDISetNoteLyrics; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const uint64_t ref;
	const int track, index;
	const juce::String lyrics;

	juce::String oldLyrics;

	JUCE_LEAK_DETECTOR(ActionMIDISetNoteLyrics)
};

class ActionMIDIRemoveNote final : public ActionUndoableBase {
public:
	ActionMIDIRemoveNote() = delete;
	ActionMIDIRemoveNote(
		uint64_t ref, int track, int index);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "MIDI Remove Note";
	};
	ActionType getActionType() const override { return ActionType::ActionMIDIRemoveNote; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const uint64_t ref;
	const int track;
	const int index;

	double startTime = 0, endTime = 0;
	uint8_t channel = 0;
	uint8_t pitch = 0;
	uint8_t vel = 0;
	juce::String lyrics;

	int oldEndIndex = -1;

	JUCE_LEAK_DETECTOR(ActionMIDIAddNote)
};
