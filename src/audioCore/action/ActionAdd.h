#pragma once

#include "ActionBase.h"
#include "../quickAPI/QuickGet.h"

class ActionAddPluginBlackList final : public ActionBase {
public:
	ActionAddPluginBlackList() = delete;
	ActionAddPluginBlackList(const juce::String& plugin);

	bool doAction() override;
	const juce::String getName() const override {
		return "Add Plugin Black List";
	};
	int getShieldMask() const override {
		return ShieldPluginLoad | ShieldPluginScan;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String plugin;

	JUCE_LEAK_DETECTOR(ActionAddPluginBlackList)
};

class ActionAddPluginSearchPath final : public ActionBase {
public:
	ActionAddPluginSearchPath() = delete;
	ActionAddPluginSearchPath(const juce::String& path);

	bool doAction() override;
	const juce::String getName() const override {
		return "Add Plugin Search Path";
	};
	int getShieldMask() const override {
		return ShieldPluginLoad | ShieldPluginScan;
	};
	const juce::String getStatusStr() const override;

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
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Track";
	};
	ActionType getActionType() const override { return ActionType::ActionAddTrack; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int bus;

	int newIndex = -1;

	JUCE_LEAK_DETECTOR(ActionAddTrack)
};

class ActionAddTrackAudioInput final : public ActionUndoableBase {
public:
	ActionAddTrackAudioInput() = delete;
	ActionAddTrackAudioInput(
		quickAPI::TrackIndex index,
		int srcc, int dstc);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Track Audio Input";
	};
	ActionType getActionType() const override { return ActionType::ActionAddTrackAudioInput; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int srcc, dstc;

	JUCE_LEAK_DETECTOR(ActionAddTrackAudioInput)
};

class ActionAddTrackMIDIInput final : public ActionUndoableBase {
public:
	ActionAddTrackMIDIInput() = delete;
	ActionAddTrackMIDIInput(
		quickAPI::TrackIndex index);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Track MIDI Input";
	};
	ActionType getActionType() const override { return ActionType::ActionAddTrackMIDIInput; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;

	JUCE_LEAK_DETECTOR(ActionAddTrackMIDIInput)
};

class ActionAddTrackAudioSend final : public ActionUndoableBase {
public:
	ActionAddTrackAudioSend() = delete;
	ActionAddTrackAudioSend(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst, int srcc, int dstc);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Track Audio Send";
	};
	ActionType getActionType() const override { return ActionType::ActionAddTrackAudioSend; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int slot;
	const quickAPI::SendDst dst;
	const int srcc, dstc;

	JUCE_LEAK_DETECTOR(ActionAddTrackAudioSend)
};

class ActionAddTrackMIDISend final : public ActionUndoableBase {
public:
	ActionAddTrackMIDISend() = delete;
	ActionAddTrackMIDISend(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Track MIDI Send";
	};
	ActionType getActionType() const override { return ActionType::ActionAddTrackMIDISend; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int slot;
	const quickAPI::SendDst dst;

	JUCE_LEAK_DETECTOR(ActionAddTrackMIDISend)
};

class ActionAddEffect final : public ActionUndoableBase {
public:
	ActionAddEffect() = delete;
	ActionAddEffect(
		quickAPI::TrackIndex trackIndex,
		int effect, const juce::String& pid);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Effect";
	};
	int getShieldMask() const override {
		return ShieldRendering | ShieldPluginScan;
	};
	ActionType getActionType() const override { return ActionType::ActionAddEffect; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex trackIndex;
	const int effect;
	const juce::String pid;

	JUCE_LEAK_DETECTOR(ActionAddEffect)
};

class ActionAddInstr final : public ActionUndoableBase {
public:
	ActionAddInstr() = delete;
	ActionAddInstr(
		int index, const juce::String& pid, bool addARA);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Instr";
	};
	int getShieldMask() const override {
		return ShieldRendering | ShieldPluginScan;
	};
	ActionType getActionType() const override { return ActionType::ActionAddInstr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int index;
	const juce::String pid;
	const bool addARA;

	JUCE_LEAK_DETECTOR(ActionAddInstr)
};

class ActionAddTrackSideChainBus final : public ActionUndoableBase {
public:
	ActionAddTrackSideChainBus() = delete;
	ActionAddTrackSideChainBus(
		quickAPI::TrackIndex index);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Track Side Chain Bus";
	};
	ActionType getActionType() const override { return ActionType::ActionAddTrackSideChainBus; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;

	JUCE_LEAK_DETECTOR(ActionAddTrackSideChainBus)
};

class ActionAddSequencerBlock final : public ActionUndoableBase {
public:
	ActionAddSequencerBlock() = delete;
	ActionAddSequencerBlock(
		int trackIndex, double startTime, double endTime, double offset);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Sequencer Block";
	};
	ActionType getActionType() const override { return ActionType::ActionAddSequencerBlock; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int trackIndex;
	const double startTime, endTime, offset;

	int index = -1;

	JUCE_LEAK_DETECTOR(ActionAddSequencerBlock)
};

class ActionAddLabelTempo final : public ActionUndoableBase {
public:
	ActionAddLabelTempo() = delete;
	ActionAddLabelTempo(
		double time, double tempo);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Tempo Label";
	};
	ActionType getActionType() const override { return ActionType::ActionAddLabelTempo; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const double time, tempo;
	int index = -1;

	JUCE_LEAK_DETECTOR(ActionAddLabelTempo)
};

class ActionAddLabelBeat final : public ActionUndoableBase {
public:
	ActionAddLabelBeat() = delete;
	ActionAddLabelBeat(
		double time, int numerator, int denominator);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Add Beat Label";
	};
	ActionType getActionType() const override { return ActionType::ActionAddLabelBeat; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const double time;
	const int numerator, denominator;
	int index = -1;

	JUCE_LEAK_DETECTOR(ActionAddLabelBeat)
};
