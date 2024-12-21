#pragma once

#include "ActionUndoableBase.h"
#include "ActionUtils.h"
#include "../graph/MainGraph.h"
#include "../quickAPI/QuickGet.h"
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

class ActionRemoveTrack final : public ActionUndoableBase {
public:
	ActionRemoveTrack() = delete;
	ActionRemoveTrack(quickAPI::TrackIndex index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Track";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;

		bool midiInput;
		MainGraph::AudioChannelLinkList audioInput;
		juce::Array<MainGraph::SendDst> midiSend;
		juce::Array<std::pair<MainGraph::SendDst, MainGraph::AudioChannelLinkList>> audioSend;
		juce::MemoryBlock data;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTrack)
};

class ActionRemoveTrackAudioInput final : public ActionUndoableBase {
public:
	ActionRemoveTrackAudioInput() = delete;
	ActionRemoveTrackAudioInput(
		quickAPI::TrackIndex index, int srcc, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Track Audio Input";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int srcc, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackAudioInput)
};

class ActionRemoveTrackMIDIInput final : public ActionUndoableBase {
public:
	ActionRemoveTrackMIDIInput() = delete;
	ActionRemoveTrackMIDIInput(
		quickAPI::TrackIndex index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Track MIDI Input";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackMIDIInput)
};

class ActionRemoveTrackAudioSend final : public ActionUndoableBase {
public:
	ActionRemoveTrackAudioSend() = delete;
	ActionRemoveTrackAudioSend(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst, int srcc, int dstc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Track Audio Send";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int slot;
		const quickAPI::SendDst dst;
		const int srcc, dstc;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackAudioSend)
};

class ActionRemoveTrackMIDISend final : public ActionUndoableBase {
public:
	ActionRemoveTrackMIDISend() = delete;
	ActionRemoveTrackMIDISend(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Track MIDI Send";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int slot;
		const quickAPI::SendDst dst;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackMIDISend)
};

class ActionRemoveTrackAudioSendAllChannel final : public ActionUndoableBase {
public:
	ActionRemoveTrackAudioSendAllChannel() = delete;
	ActionRemoveTrackAudioSendAllChannel(
		quickAPI::TrackIndex index, int slot,
		quickAPI::SendDst dst);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Track Audio Send All Channel";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int slot;
		const quickAPI::SendDst dst;
		
		MainGraph::AudioChannelLinkList channels;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackAudioSendAllChannel)
};

class ActionRemoveTrackAudioSendOnSlot final : public ActionUndoableBase {
public:
	ActionRemoveTrackAudioSendOnSlot() = delete;
	ActionRemoveTrackAudioSendOnSlot(
		quickAPI::TrackIndex index, int slot);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Track Audio Send On Slot";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int slot;

		quickAPI::SendDst dst;
		 MainGraph::AudioChannelLinkList channels;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackAudioSendOnSlot)
};

class ActionRemoveTrackMIDISendOnSlot final : public ActionUndoableBase {
public:
	ActionRemoveTrackMIDISendOnSlot() = delete;
	ActionRemoveTrackMIDISendOnSlot(
		quickAPI::TrackIndex index, int slot);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Track MIDI Send On Slot";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
		const int slot;

		quickAPI::SendDst dst;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackMIDISendOnSlot)
};

class ActionRemoveEffect final : public ActionUndoableBase {
public:
	ActionRemoveEffect() = delete;
	ActionRemoveEffect(
		quickAPI::TrackIndex trackIndex, int effect);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Effect";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex trackIndex;
		const int effect;

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

class ActionRemoveTrackSideChainBus final : public ActionUndoableBase {
public:
	ActionRemoveTrackSideChainBus() = delete;
	ActionRemoveTrackSideChainBus(quickAPI::TrackIndex index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Track Side Chain Bus";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex index;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTrackSideChainBus)
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
		quickAPI::TrackIndex track, int effect, int cc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Effect Param CC Connection";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::TrackIndex track;
		const int effect, cc;
		int param = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveEffectParamCCConnection)
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

class ActionRemoveTempo final : public ActionUndoableBase {
public:
	ActionRemoveTempo() = delete;
	ActionRemoveTempo(int index);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Remove Tempo";
	};

private:
	ACTION_DATABLOCK{
		const int index;
		double time = 0, tempo = 120;
		int numerator = 4, denominator = 4;
		bool isTempo = false;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionRemoveTempo)
};
