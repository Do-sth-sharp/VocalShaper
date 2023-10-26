#pragma once

#include "ActionUndoableBase.h"
#include "../Utils.h"

class ActionRemovePluginBlackList final : public ActionBase {
public:
	ActionRemovePluginBlackList() = delete;
	ActionRemovePluginBlackList(const juce::String& path);

	bool doAction() override;

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionRemovePluginBlackList)
};

class ActionRemovePluginSearchPath final : public ActionBase {
public:
	ActionRemovePluginSearchPath() = delete;
	ActionRemovePluginSearchPath(const juce::String& path);

	bool doAction() override;

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

private:
	const int track, effect;

	utils::AudioConnectionList additional;
	juce::MemoryBlock data;

	JUCE_LEAK_DETECTOR(ActionRemoveEffect)
};
