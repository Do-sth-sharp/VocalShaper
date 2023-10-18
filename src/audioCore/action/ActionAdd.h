﻿#pragma once

#include "ActionUndoableBase.h"

class ActionAddPluginBlackList final : public ActionBase {
public:
	ActionAddPluginBlackList() = delete;
	ActionAddPluginBlackList(const juce::String& plugin);

	bool doAction() override;

private:
	const juce::String plugin;

	JUCE_LEAK_DETECTOR(ActionAddPluginBlackList)
};

class ActionAddPluginSearchPath final : public ActionBase {
public:
	ActionAddPluginSearchPath() = delete;
	ActionAddPluginSearchPath(const juce::String& path);

	bool doAction() override;

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

private:
	const int src, srcc, dstc;

	JUCE_LEAK_DETECTOR(ActionAddMixerTrackOutput)
};