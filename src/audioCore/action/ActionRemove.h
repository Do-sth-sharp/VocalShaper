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
