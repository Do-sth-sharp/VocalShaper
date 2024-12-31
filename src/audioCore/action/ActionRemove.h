#pragma once

#include "ActionBase.h"
#include "../graph/MainGraph.h"
#include "../quickAPI/QuickGet.h"
#include "../Utils.h"

class ActionRemovePluginBlackList final : public ActionBase {
public:
	ActionRemovePluginBlackList() = delete;
	ActionRemovePluginBlackList(const juce::String& path);

	bool doAction() override;
	const juce::String getName() const override {
		return "Remove Plugin Black List";
	};
	int getShieldMask() const override {
		return ShieldPluginLoad | ShieldPluginScan;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionRemovePluginBlackList)
};

class ActionRemovePluginSearchPath final : public ActionBase {
public:
	ActionRemovePluginSearchPath() = delete;
	ActionRemovePluginSearchPath(const juce::String& path);

	bool doAction() override;
	const juce::String getName() const override {
		return "Remove Plugin Search Path";
	};
	int getShieldMask() const override {
		return ShieldPluginLoad | ShieldPluginScan;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionRemovePluginSearchPath)
};

class ActionRemoveTrack final : public ActionUndoableBase {
public:
	ActionRemoveTrack() = delete;
	ActionRemoveTrack(quickAPI::TrackIndex index);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Track";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveTrack; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;

	bool midiInput;
	MainGraph::AudioChannelLinkList audioInput;
	juce::Array<MainGraph::SendDst> midiSend;
	juce::Array<std::pair<MainGraph::SendDst, MainGraph::AudioChannelLinkList>> audioSend;
	juce::MemoryBlock data;

	JUCE_LEAK_DETECTOR(ActionRemoveTrack)
};

class ActionRemoveTrackAudioInput final : public ActionUndoableBase {
public:
	ActionRemoveTrackAudioInput() = delete;
	ActionRemoveTrackAudioInput(
		quickAPI::TrackIndex index, int srcc, int dstc);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Track Audio Input";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveTrackAudioInput; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int srcc, dstc;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackAudioInput)
};

class ActionRemoveTrackMIDIInput final : public ActionUndoableBase {
public:
	ActionRemoveTrackMIDIInput() = delete;
	ActionRemoveTrackMIDIInput(
		quickAPI::TrackIndex index);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Track MIDI Input";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveTrackMIDIInput; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackMIDIInput)
};

class ActionRemoveTrackAudioSend final : public ActionUndoableBase {
public:
	ActionRemoveTrackAudioSend() = delete;
	ActionRemoveTrackAudioSend(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst, int srcc, int dstc);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Track Audio Send";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveTrackAudioSend; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int slot;
	const quickAPI::SendDst dst;
	const int srcc, dstc;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackAudioSend)
};

class ActionRemoveTrackMIDISend final : public ActionUndoableBase {
public:
	ActionRemoveTrackMIDISend() = delete;
	ActionRemoveTrackMIDISend(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Track MIDI Send";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveTrackMIDISend; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int slot;
	const quickAPI::SendDst dst;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackMIDISend)
};

class ActionRemoveTrackAudioSendAllChannel final : public ActionUndoableBase {
public:
	ActionRemoveTrackAudioSendAllChannel() = delete;
	ActionRemoveTrackAudioSendAllChannel(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Track Audio Send All Channel";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveTrackAudioSendAllChannel; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int slot;
	const quickAPI::SendDst dst;

	MainGraph::AudioChannelLinkList channels;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackAudioSendAllChannel)
};

class ActionRemoveTrackAudioSendOnSlot final : public ActionUndoableBase {
public:
	ActionRemoveTrackAudioSendOnSlot() = delete;
	ActionRemoveTrackAudioSendOnSlot(
		quickAPI::TrackIndex index, int slot);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Track Audio Send On Slot";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveTrackAudioSendOnSlot; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int slot;

	quickAPI::SendDst dst;
	MainGraph::AudioChannelLinkList channels;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackAudioSendOnSlot)
};

class ActionRemoveTrackMIDISendOnSlot final : public ActionUndoableBase {
public:
	ActionRemoveTrackMIDISendOnSlot() = delete;
	ActionRemoveTrackMIDISendOnSlot(
		quickAPI::TrackIndex index, int slot);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Track MIDI Send On Slot";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveTrackMIDISendOnSlot; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;
	const int slot;

	quickAPI::SendDst dst;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackMIDISendOnSlot)
};

class ActionRemoveEffect final : public ActionUndoableBase {
public:
	ActionRemoveEffect() = delete;
	ActionRemoveEffect(
		quickAPI::TrackIndex trackIndex, int effect);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Effect";
	};
	int getShieldMask() const override {
		return ShieldRendering | ShieldPluginScan;
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveEffect; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex trackIndex;
	const int effect;

	juce::MemoryBlock data;

	JUCE_LEAK_DETECTOR(ActionRemoveEffect)
};

class ActionRemoveInstr final : public ActionUndoableBase {
public:
	ActionRemoveInstr() = delete;
	ActionRemoveInstr(int index);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Instr";
	};
	int getShieldMask() const override {
		return ShieldRendering | ShieldPluginScan;
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveInstr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int index;

	juce::MemoryBlock data;

	JUCE_LEAK_DETECTOR(ActionRemoveInstr)
};

class ActionRemoveTrackSideChainBus final : public ActionUndoableBase {
public:
	ActionRemoveTrackSideChainBus() = delete;
	ActionRemoveTrackSideChainBus(quickAPI::TrackIndex index);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Track Side Chain Bus";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveTrackSideChainBus; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex index;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackSideChainBus)
};

class ActionRemoveInstrParamCCConnection final : public ActionUndoableBase {
public:
	ActionRemoveInstrParamCCConnection() = delete;
	ActionRemoveInstrParamCCConnection(
		int instr, int cc);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Instr Param CC Connection";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveInstrParamCCConnection; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int instr, cc;
	int param = -1;

	JUCE_LEAK_DETECTOR(ActionRemoveInstrParamCCConnection)
};

class ActionRemoveEffectParamCCConnection final : public ActionUndoableBase {
public:
	ActionRemoveEffectParamCCConnection() = delete;
	ActionRemoveEffectParamCCConnection(
		quickAPI::TrackIndex track, int effect, int cc);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Effect Param CC Connection";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveEffectParamCCConnection; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const int effect, cc;
	int param = -1;

	JUCE_LEAK_DETECTOR(ActionRemoveEffectParamCCConnection)
};

class ActionRemoveSequencerBlock final : public ActionUndoableBase {
public:
	ActionRemoveSequencerBlock() = delete;
	ActionRemoveSequencerBlock(
		int seqIndex, int index);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Sequencer Block";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveSequencerBlock; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int seqIndex, index;
	double startTime = 0, endTime = 0, offset = 0;

	JUCE_LEAK_DETECTOR(ActionRemoveSequencerBlock)
};

class ActionRemoveLabel final : public ActionUndoableBase {
public:
	ActionRemoveLabel() = delete;
	ActionRemoveLabel(int index);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Remove Tempo";
	};
	ActionType getActionType() const override { return ActionType::ActionRemoveLabel; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int index;

	double time = 0, tempo = 120;
	int numerator = 4, denominator = 4;
	bool isTempo = false;

	JUCE_LEAK_DETECTOR(ActionRemoveLabel)
};
