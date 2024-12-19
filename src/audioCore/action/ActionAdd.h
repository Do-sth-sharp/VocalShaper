#pragma once

#include "ActionUndoableBase.h"
#include "ActionUtils.h"
#include "../quickAPI/QuickGet.h"

class ActionAddPluginBlackList final : public ActionBase {
public:
	ActionAddPluginBlackList() = delete;
	ActionAddPluginBlackList(const juce::String& plugin);

	bool doAction() override;
	const juce::String getName() override {
		return "Add Plugin Black List";
	};

private:
	const juce::String plugin;

	JUCE_LEAK_DETECTOR(ActionAddPluginBlackList)
};

class ActionAddPluginSearchPath final : public ActionBase {
public:
	ActionAddPluginSearchPath() = delete;
	ActionAddPluginSearchPath(const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "Add Plugin Search Path";
	};

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionAddPluginSearchPath)
};

class ActionAddTrack final : public ActionUndoableBase {
public:
	ActionAddTrack() = delete;
	ActionAddTrack(
		quickAPI::TrackIndex index, int bus = -1);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Track";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int bus;
		int newIndex = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddTrack)
};

class ActionAddTrackAudioInput final : public ActionUndoableBase {
public:
	ActionAddTrackAudioInput() = delete;
	ActionAddTrackAudioInput(
		quickAPI::TrackIndex index,
		int srcc, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Track Audio Input";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int srcc, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddTrackAudioInput)
};

class ActionAddTrackMIDIInput final : public ActionUndoableBase {
public:
	ActionAddTrackMIDIInput() = delete;
	ActionAddTrackMIDIInput(
		quickAPI::TrackIndex index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Track MIDI Input";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddTrackMIDIInput)
};

class ActionAddTrackAudioSend final : public ActionUndoableBase {
public:
	ActionAddTrackAudioSend() = delete;
	ActionAddTrackAudioSend(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst, int srcc, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Track Audio Send";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int slot;
		const quickAPI::SendDst dst;
		const int srcc, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddTrackAudioSend)
};

class ActionAddTrackMIDISend final : public ActionUndoableBase {
public:
	ActionAddTrackMIDISend() = delete;
	ActionAddTrackMIDISend(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Track MIDI Send";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int slot;
		const quickAPI::SendDst dst;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddTrackMIDISend)
};

class ActionAddEffect final : public ActionUndoableBase {
public:
	ActionAddEffect() = delete;
	ActionAddEffect(
		quickAPI::TrackIndex trackIndex,
		int effect, const juce::String& pid);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Effect";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex trackIndex;
		const int effect;
		const juce::String pid;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddEffect)
};

class ActionAddInstr final : public ActionUndoableBase {
public:
	ActionAddInstr() = delete;
	ActionAddInstr(
		int index, const juce::String& pid, bool addARA);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Instr";
	};

private:
	ACTION_DATABLOCK{
		const int index;
		const juce::String pid;
		const bool addARA;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddInstr)
};

class ActionAddTrackSideChainBus final : public ActionUndoableBase {
public:
	ActionAddTrackSideChainBus() = delete;
	ActionAddTrackSideChainBus(
		quickAPI::TrackIndex index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Track Side Chain Bus";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddTrackSideChainBus)
};

class ActionAddSequencerBlock final : public ActionUndoableBase {
public:
	ActionAddSequencerBlock() = delete;
	ActionAddSequencerBlock(
		int trackIndex, double startTime, double endTime, double offset);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Sequencer Block";
	};

private:
	ACTION_DATABLOCK{
		const int trackIndex;
		const double startTime, endTime, offset;
		int index = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddSequencerBlock)
};

class ActionAddTempoTempo final : public ActionUndoableBase {
public:
	ActionAddTempoTempo() = delete;
	ActionAddTempoTempo(
		double time, double tempo);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Tempo Label";
	};

private:
	ACTION_DATABLOCK{
		const double time, tempo;
		int index = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddTempoTempo)
};

class ActionAddTempoBeat final : public ActionUndoableBase {
public:
	ActionAddTempoBeat() = delete;
	ActionAddTempoBeat(
		double time, int numerator, int denominator);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Beat Label";
	};

private:
	ACTION_DATABLOCK{
		const double time;
		const int numerator, denominator;
		int index = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddTempoBeat)
};
