#pragma once

#include "ActionUndoableBase.h"
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
	const int index;

	utils::AudioConnectionList audioT2Trk;
	utils::AudioConnectionList audioSrc2Trk;
	utils::AudioConnectionList audioInstr2Trk;
	utils::AudioConnectionList audioI2Trk;
	utils::AudioConnectionList audioTrk2T;
	utils::AudioConnectionList audioTrk2O;
	utils::MidiConnectionList midiSrc2Trk;
	utils::MidiConnectionList midiI2Trk;
	utils::MidiConnectionList midiTrk2O;
	juce::MemoryBlock data;

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
	const int src, srcc, dst, dstc;

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
	const int srcc, dst, dstc;

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
	const int src, srcc, dstc;

	JUCE_LEAK_DETECTOR(ActionRemoveMixerTrackOutput)
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
	const int track, effect;

	utils::AudioConnectionList additional;
	juce::MemoryBlock data;

	JUCE_LEAK_DETECTOR(ActionRemoveEffect)
};

class ActionRemoveEffectAdditionalInput final : public ActionUndoableBase {
public:
	ActionRemoveEffectAdditionalInput() = delete;
	ActionRemoveEffectAdditionalInput(
		int track, int effect, int srcc, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Effect Additional Input";
	};

private:
	const int track, effect, srcc, dstc;

	JUCE_LEAK_DETECTOR(ActionRemoveEffectAdditionalInput)
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
	const int index;

	utils::AudioConnectionList audioInstr2Trk;
	utils::MidiConnectionList midiSrc2Instr;
	utils::MidiConnectionList midiI2Instr;
	juce::MemoryBlock data;

	JUCE_LEAK_DETECTOR(ActionRemoveInstr)
};

class ActionRemoveInstrOutput final : public ActionUndoableBase {
public:
	ActionRemoveInstrOutput() = delete;
	ActionRemoveInstrOutput(
		int src, int srcc, int dst, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Instr Output";
	};

private:
	const int src, srcc, dst, dstc;

	JUCE_LEAK_DETECTOR(ActionRemoveInstrOutput)
};

class ActionRemoveInstrMidiInput final : public ActionUndoableBase {
public:
	ActionRemoveInstrMidiInput() = delete;
	ActionRemoveInstrMidiInput(int dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Instr Midi Input";
	};

private:
	const int dst;

	JUCE_LEAK_DETECTOR(ActionRemoveInstrMidiInput)
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
	const int instr, cc;
	int param = -1;

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
	const int track, effect, cc;
	int param = -1;

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
	const int index;

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
	const int index;

	JUCE_LEAK_DETECTOR(ActionRemoveMixerTrackMidiOutput)
};

class ActionRemoveSource final : public ActionBase {
public:
	ActionRemoveSource() = delete;
	ActionRemoveSource(int index);

	bool doAction() override;
	const juce::String getName() override {
		return "Remove Source";
	};

private:
	const int index;

	JUCE_LEAK_DETECTOR(ActionRemoveSource)
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
	const int index;

	utils::AudioConnectionList audioSrc2Trk;
	utils::MidiConnectionList midiSrc2Instr;
	utils::MidiConnectionList midiSrc2Trk;
	juce::MemoryBlock data;

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
	const int src, dst;

	JUCE_LEAK_DETECTOR(ActionRemoveSequencerTrackMidiOutputToMixer)
};

class ActionRemoveSequencerTrackMidiOutputToInstr final : public ActionUndoableBase {
public:
	ActionRemoveSequencerTrackMidiOutputToInstr() = delete;
	ActionRemoveSequencerTrackMidiOutputToInstr(
		int src, int dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Sequencer Track Midi Output To Instr";
	};

private:
	const int src, dst;

	JUCE_LEAK_DETECTOR(ActionRemoveSequencerTrackMidiOutputToInstr)
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
	const int src, srcc, dst, dstc;

	JUCE_LEAK_DETECTOR(ActionRemoveSequencerTrackOutput)
};

class ActionRemoveSequencerSourceInstance final : public ActionUndoableBase {
public:
	ActionRemoveSequencerSourceInstance() = delete;
	ActionRemoveSequencerSourceInstance(
		int track, int seq);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Sequencer Source Instance";
	};

private:
	const int track, seq;

	int index;
	double start, end, offset;

	JUCE_LEAK_DETECTOR(ActionRemoveSequencerSourceInstance)
};

class ActionRemoveRecorderSourceInstance final : public ActionUndoableBase {
public:
	ActionRemoveRecorderSourceInstance() = delete;
	ActionRemoveRecorderSourceInstance(int seq);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Recorder Source Instance";
	};

private:
	const int seq;

	int index;
	double offset;

	JUCE_LEAK_DETECTOR(ActionRemoveRecorderSourceInstance)
};
