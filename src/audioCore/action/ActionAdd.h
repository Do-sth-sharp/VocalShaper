#pragma once

#include "ActionUndoableBase.h"
#include "ActionUtils.h"

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

class ActionAddMixerTrack final : public ActionUndoableBase {
public:
	ActionAddMixerTrack() = delete;
	ActionAddMixerTrack(int index = -1, int type = -1);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Mixer Track";
	};

private:
	ACTION_DATABLOCK{
		const int index, type;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddMixerTrack)
};

class ActionAddMixerTrackSend final : public ActionUndoableBase {
public:
	ActionAddMixerTrackSend() = delete;
	ActionAddMixerTrackSend(
		int src, int srcc, int dst, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Mixer Track Send";
	};

private:
	ACTION_DATABLOCK{
		const int src, srcc, dst, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddMixerTrack)
};

class ActionAddMixerTrackInputFromDevice final : public ActionUndoableBase {
public:
	ActionAddMixerTrackInputFromDevice() = delete;
	ActionAddMixerTrackInputFromDevice(
		int srcc, int dst, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Mixer Track Input From Device";
	};

private:
	ACTION_DATABLOCK{
		const int srcc, dst, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddMixerTrackInputFromDevice)
};

class ActionAddMixerTrackOutput final : public ActionUndoableBase {
public:
	ActionAddMixerTrackOutput() = delete;
	ActionAddMixerTrackOutput(
		int src, int srcc, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Mixer Track Output";
	};

private:
	ACTION_DATABLOCK{
		const int src, srcc, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddMixerTrackOutput)
};

class ActionAddEffect final : public ActionUndoableBase {
public:
	ActionAddEffect() = delete;
	ActionAddEffect(
		int track, int effect, const juce::String& pid);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Effect";
	};

private:
	ACTION_DATABLOCK{
		const int track, effect;
		const juce::String pid;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddEffect)
};

class ActionAddInstr final : public ActionUndoableBase {
public:
	ActionAddInstr() = delete;
	ActionAddInstr(
		int index, const juce::String& pid);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Instr";
	};

private:
	ACTION_DATABLOCK{
		const int index;
		const juce::String pid;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddInstr)
};

class ActionAddMixerTrackMidiInput final : public ActionUndoableBase {
public:
	ActionAddMixerTrackMidiInput() = delete;
	ActionAddMixerTrackMidiInput(int dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Mixer Track Midi Input";
	};

private:
	ACTION_DATABLOCK{
		const int dst;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddMixerTrackMidiInput)
};

class ActionAddMixerTrackMidiOutput final : public ActionUndoableBase {
public:
	ActionAddMixerTrackMidiOutput() = delete;
	ActionAddMixerTrackMidiOutput(int src);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Mixer Track Midi Output";
	};

private:
	ACTION_DATABLOCK{
		const int src;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddMixerTrackMidiOutput)
};

class ActionAddMixerTrackSideChainBus final : public ActionUndoableBase {
public:
	ActionAddMixerTrackSideChainBus() = delete;
	ActionAddMixerTrackSideChainBus(int track);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Mixer Track Side Chain Bus";
	};

private:
	ACTION_DATABLOCK{
		const int track;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddMixerTrackSideChainBus)
};

class ActionAddSequencerTrack final : public ActionUndoableBase {
public:
	ActionAddSequencerTrack() = delete;
	ActionAddSequencerTrack(
		int index, int type);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Sequencer Track";
	};

private:
	ACTION_DATABLOCK{
		const int index, type;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddSequencerTrack)
};

class ActionAddSequencerTrackMidiOutputToMixer final : public ActionUndoableBase {
public:
	ActionAddSequencerTrackMidiOutputToMixer() = delete;
	ActionAddSequencerTrackMidiOutputToMixer(
		int src, int dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Sequencer Track Midi Output To Mixer";
	};

private:
	ACTION_DATABLOCK{
		const int src, dst;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddSequencerTrackMidiOutputToMixer)
};

class ActionAddSequencerTrackOutput final : public ActionUndoableBase {
public:
	ActionAddSequencerTrackOutput() = delete;
	ActionAddSequencerTrackOutput(
		int src, int srcc, int dst, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Sequencer Track Output";
	};

private:
	ACTION_DATABLOCK{
		const int src, srcc, dst, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionAddSequencerTrackOutput)
};
