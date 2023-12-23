#pragma once

#include "ActionUndoableBase.h"

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
	const int index, type;

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
	const int src, srcc, dst, dstc;

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
	const int srcc, dst, dstc;

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
	const int src, srcc, dstc;

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
	const int track, effect;
	const juce::String pid;

	JUCE_LEAK_DETECTOR(ActionAddEffect)
};

class ActionAddEffectAdditionalInput final : public ActionUndoableBase {
public:
	ActionAddEffectAdditionalInput() = delete;
	ActionAddEffectAdditionalInput(
		int track, int effect, int srcc, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Effect Additional Input";
	};

private:
	const int track, effect, srcc, dstc;

	JUCE_LEAK_DETECTOR(ActionAddEffectAdditionalInput)
};

class ActionAddInstr final : public ActionUndoableBase {
public:
	ActionAddInstr() = delete;
	ActionAddInstr(
		int index, int type, const juce::String& pid);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Instr";
	};

private:
	const int index, type;
	const juce::String pid;

	JUCE_LEAK_DETECTOR(ActionAddInstr)
};

class ActionAddInstrOutput final : public ActionUndoableBase {
public:
	ActionAddInstrOutput() = delete;
	ActionAddInstrOutput(
		int src, int srcc, int dst, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Instr Output";
	};

private:
	const int src, srcc, dst, dstc;

	JUCE_LEAK_DETECTOR(ActionAddInstrOutput)
};

class ActionAddInstrMidiInput final : public ActionUndoableBase {
public:
	ActionAddInstrMidiInput() = delete;
	ActionAddInstrMidiInput(int dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Instr Midi Input";
	};

private:
	const int dst;

	JUCE_LEAK_DETECTOR(ActionAddInstrMidiInput)
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
	const int dst;

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
	const int src;

	JUCE_LEAK_DETECTOR(ActionAddMixerTrackMidiOutput)
};

class ActionAddAudioSourceThenLoad final : public ActionUndoableBase {
public:
	ActionAddAudioSourceThenLoad() = delete;
	ActionAddAudioSourceThenLoad(
		const juce::String& path, bool copy);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Audio Source Then Load";
	};

private:
	const juce::String path;
	const bool copy;

	JUCE_LEAK_DETECTOR(ActionAddAudioSourceThenLoad)
};

class ActionAddAudioSourceThenInit final : public ActionUndoableBase {
public:
	ActionAddAudioSourceThenInit() = delete;
	ActionAddAudioSourceThenInit(
		double sampleRate, int channels, double length);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Audio Source Then Init";
	};

private:
	const double sampleRate, length;
	const int channels;

	JUCE_LEAK_DETECTOR(ActionAddAudioSourceThenInit)
};

class ActionAddMidiSourceThenLoad final : public ActionUndoableBase {
public:
	ActionAddMidiSourceThenLoad() = delete;
	ActionAddMidiSourceThenLoad(
		const juce::String& path, bool copy);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Midi Source Then Load";
	};

private:
	const juce::String path;
	const bool copy;

	JUCE_LEAK_DETECTOR(ActionAddMidiSourceThenLoad)
};

class ActionAddMidiSourceThenInit final : public ActionUndoableBase {
public:
	ActionAddMidiSourceThenInit();

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Midi Source Then Init";
	};

private:
	JUCE_LEAK_DETECTOR(ActionAddMidiSourceThenInit)
};

class ActionAddSynthSourceThenLoad final : public ActionUndoableBase {
public:
	ActionAddSynthSourceThenLoad() = delete;
	ActionAddSynthSourceThenLoad(
		const juce::String& path, bool copy);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Synth Source Then Load";
	};

private:
	const juce::String path;
	const bool copy;

	JUCE_LEAK_DETECTOR(ActionAddSynthSourceThenLoad)
};

class ActionAddSynthSourceThenInit final : public ActionUndoableBase {
public:
	ActionAddSynthSourceThenInit();

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Synth Source Then Init";
	};

private:
	JUCE_LEAK_DETECTOR(ActionAddSynthSourceThenInit)
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
	const int index, type;

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
	const int src, dst;

	JUCE_LEAK_DETECTOR(ActionAddSequencerTrackMidiOutputToMixer)
};

class ActionAddSequencerTrackMidiOutputToInstr final : public ActionUndoableBase {
public:
	ActionAddSequencerTrackMidiOutputToInstr() = delete;
	ActionAddSequencerTrackMidiOutputToInstr(
		int src, int dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Sequencer Track Midi Output To Instr";
	};

private:
	const int src, dst;

	JUCE_LEAK_DETECTOR(ActionAddSequencerTrackMidiOutputToInstr)
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
	const int src, srcc, dst, dstc;

	JUCE_LEAK_DETECTOR(ActionAddSequencerTrackOutput)
};

class ActionAddSequencerSourceInstance final : public ActionUndoableBase {
public:
	ActionAddSequencerSourceInstance() = delete;
	ActionAddSequencerSourceInstance(
		int track, int src, double start, double end, double offset);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Sequencer Source Instance";
	};

private:
	const int track, src;
	const double start, end, offset;
	int index = -1;

	JUCE_LEAK_DETECTOR(ActionAddSequencerSourceInstance)
};

class ActionAddRecorderSourceInstance final : public ActionUndoableBase {
public:
	ActionAddRecorderSourceInstance() = delete;
	ActionAddRecorderSourceInstance(
		int src, double offset, int compensate);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Add Recorder Source Instance";
	};

private:
	const int src;
	const double offset;
	const int compensate;

	JUCE_LEAK_DETECTOR(ActionAddRecorderSourceInstance)
};
