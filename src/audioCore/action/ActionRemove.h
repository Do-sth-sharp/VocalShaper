#pragma once

#include "ActionUndoableBase.h"
#include "ActionUtils.h"
#include "../Utils.h"

class ActionRemovePluginBlackList final : public ActionBase {
public:
	ActionRemovePluginBlackList() = delete;
	ActionRemovePluginBlackList(const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "Remove Plugin Black List";
	};

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionRemovePluginBlackList)
};

class ActionRemovePluginSearchPath final : public ActionBase {
public:
	ActionRemovePluginSearchPath() = delete;
	ActionRemovePluginSearchPath(const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "Remove Plugin Search Path";
	};

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionRemovePluginSearchPath)
};

class ActionRemoveMixerTrack final : public ActionUndoableBase {
public:
	ActionRemoveMixerTrack() = delete;
	ActionRemoveMixerTrack(int index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Mixer Track";
	};

private:
	ACTION_DATABLOCK{
		const int index;

		utils::AudioConnectionList audioT2Trk;
		utils::AudioConnectionList audioSrc2Trk;
		utils::AudioConnectionList audioI2Trk;
		utils::AudioConnectionList audioTrk2T;
		utils::AudioConnectionList audioTrk2O;
		utils::MidiConnectionList midiSrc2Trk;
		utils::MidiConnectionList midiI2Trk;
		utils::MidiConnectionList midiTrk2O;
		juce::MemoryBlock data;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveMixerTrack)
};

class ActionRemoveMixerTrackSend final : public ActionUndoableBase {
public:
	ActionRemoveMixerTrackSend() = delete;
	ActionRemoveMixerTrackSend(
		int src, int srcc, int dst, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Mixer Track Send";
	};

private:
	ACTION_DATABLOCK{
		const int src, srcc, dst, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveMixerTrackSend)
};

class ActionRemoveMixerTrackInputFromDevice final : public ActionUndoableBase {
public:
	ActionRemoveMixerTrackInputFromDevice() = delete;
	ActionRemoveMixerTrackInputFromDevice(
		int srcc, int dst, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Mixer Track Input From Device";
	};

private:
	ACTION_DATABLOCK{
		const int srcc, dst, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveMixerTrackInputFromDevice)
};

class ActionRemoveMixerTrackOutput final : public ActionUndoableBase {
public:
	ActionRemoveMixerTrackOutput() = delete;
	ActionRemoveMixerTrackOutput(
		int src, int srcc, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Mixer Track Output";
	};

private:
	ACTION_DATABLOCK{
		const int src, srcc, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveMixerTrackOutput)
};

class ActionRemoveMixerTrackSideChainBus final : public ActionUndoableBase {
public:
	ActionRemoveMixerTrackSideChainBus() = delete;
	ActionRemoveMixerTrackSideChainBus(int track);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Mixer Track Side Chain Bus";
	};

private:
	ACTION_DATABLOCK{
		const int track;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveMixerTrackSideChainBus)
};

class ActionRemoveEffect final : public ActionUndoableBase {
public:
	ActionRemoveEffect() = delete;
	ActionRemoveEffect(
		int track, int effect);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Effect";
	};

private:
	ACTION_DATABLOCK{
		const int track, effect;
		juce::MemoryBlock data;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveEffect)
};

class ActionRemoveInstr final : public ActionUndoableBase {
public:
	ActionRemoveInstr() = delete;
	ActionRemoveInstr(int index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Instr";
	};

private:
	ACTION_DATABLOCK{
		const int index;
		juce::MemoryBlock data;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveInstr)
};

class ActionRemoveInstrParamCCConnection final : public ActionUndoableBase {
public:
	ActionRemoveInstrParamCCConnection() = delete;
	ActionRemoveInstrParamCCConnection(
		int instr, int cc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Instr Param CC Connection";
	};

private:
	ACTION_DATABLOCK{
		const int instr, cc;
		int param = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveInstrParamCCConnection)
};

class ActionRemoveEffectParamCCConnection final : public ActionUndoableBase {
public:
	ActionRemoveEffectParamCCConnection() = delete;
	ActionRemoveEffectParamCCConnection(
		int track, int effect, int cc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Effect Param CC Connection";
	};

private:
	ACTION_DATABLOCK{
		const int track, effect, cc;
		int param = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveEffectParamCCConnection)
};

class ActionRemoveMixerTrackMidiInput final : public ActionUndoableBase {
public:
	ActionRemoveMixerTrackMidiInput() = delete;
	ActionRemoveMixerTrackMidiInput(int index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Mixer Track Midi Input";
	};

private:
	ACTION_DATABLOCK{
		const int index;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveMixerTrackMidiInput)
};

class ActionRemoveMixerTrackMidiOutput final : public ActionUndoableBase {
public:
	ActionRemoveMixerTrackMidiOutput() = delete;
	ActionRemoveMixerTrackMidiOutput(int index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Mixer Track Midi Output";
	};

private:
	ACTION_DATABLOCK{
		const int index;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveMixerTrackMidiOutput)
};

class ActionRemoveSequencerTrack final : public ActionUndoableBase {
public:
	ActionRemoveSequencerTrack() = delete;
	ActionRemoveSequencerTrack(int index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Sequencer Track";
	};

private:
	ACTION_DATABLOCK{
		const int index;

		utils::AudioConnectionList audioSrc2Trk;
		utils::MidiConnectionList midiSrc2Trk;
		juce::MemoryBlock data;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveSequencerTrack)
};

class ActionRemoveSequencerTrackMidiOutputToMixer final : public ActionUndoableBase {
public:
	ActionRemoveSequencerTrackMidiOutputToMixer() = delete;
	ActionRemoveSequencerTrackMidiOutputToMixer(
		int src, int dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Sequencer Track Midi Output To Mixer";
	};

private:
	ACTION_DATABLOCK{
		const int src, dst;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveSequencerTrackMidiOutputToMixer)
};

class ActionRemoveSequencerTrackOutput final : public ActionUndoableBase {
public:
	ActionRemoveSequencerTrackOutput() = delete;
	ActionRemoveSequencerTrackOutput(
		int src, int srcc, int dst, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Sequencer Track Output";
	};

private:
	ACTION_DATABLOCK{
		const int src, srcc, dst, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveSequencerTrackOutput)
};

class ActionRemoveSequencerBlock final : public ActionUndoableBase {
public:
	ActionRemoveSequencerBlock() = delete;
	ActionRemoveSequencerBlock(
		int seqIndex, int index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Sequencer Block";
	};

private:
	ACTION_DATABLOCK{
		const int seqIndex, index;
		double startTime = 0, endTime = 0, offset = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveSequencerBlock)
};
