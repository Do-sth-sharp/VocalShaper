#pragma once

#include "ActionUndoableBase.h"
#include "ActionUtils.h"
#include "../quickAPI/QuickGet.h"

class ActionClearPlugin final : public ActionBase {
public:
	ActionClearPlugin();

	bool doAction() override;
	const juce::String getName() override {
		return "Clear Plugin";
	};

private:
	JUCE_LEAK_DETECTOR(ActionClearPlugin)
};

class ActionSearchPlugin final : public ActionBase {
public:
	ActionSearchPlugin();

	bool doAction() override;
	const juce::String getName() override {
		return "Search Plugin";
	};

private:
	JUCE_LEAK_DETECTOR(ActionSearchPlugin)
};

class ActionPlay final : public ActionBase {
public:
	ActionPlay();

	bool doAction() override;
	const juce::String getName() override {
		return "Play";
	};

private:
	JUCE_LEAK_DETECTOR(ActionPlay)
};

class ActionPause final : public ActionBase {
public:
	ActionPause();

	bool doAction() override;
	const juce::String getName() override {
		return "Pause";
	};

private:
	JUCE_LEAK_DETECTOR(ActionPause)
};

class ActionStop final : public ActionBase {
public:
	ActionStop();

	bool doAction() override;
	const juce::String getName() override {
		return "Stop";
	};

private:
	JUCE_LEAK_DETECTOR(ActionStop)
};

class ActionRewind final : public ActionBase {
public:
	ActionRewind();

	bool doAction() override;
	const juce::String getName() override {
		return "Rewind";
	};

private:
	JUCE_LEAK_DETECTOR(ActionRewind)
};

class ActionStartRecord final : public ActionBase {
public:
	ActionStartRecord();

	bool doAction() override;
	const juce::String getName() override {
		return "Start Record";
	};

private:
	JUCE_LEAK_DETECTOR(ActionStartRecord)
};

class ActionStopRecord final : public ActionBase {
public:
	ActionStopRecord();

	bool doAction() override;
	const juce::String getName() override {
		return "Stop Record";
	};

private:
	JUCE_LEAK_DETECTOR(ActionStopRecord)
};

class ActionRenderNow final : public ActionBase {
public:
	ActionRenderNow() = delete;
	ActionRenderNow(
		const juce::String& path, const juce::String& name,
		const juce::String& extension, const juce::Array<int>& tracks,
		const juce::StringPairArray& metaData, int bitDepth, int quality);

	bool doAction() override;
	const juce::String getName() override {
		return "Render Now";
	};

private:
	const juce::String path, name, extension;
	const juce::Array<int> tracks;
	const juce::StringPairArray metaData;
	const int bitDepth;
	const int quality;

	JUCE_LEAK_DETECTOR(ActionRenderNow)
};

class ActionNewProject final : public ActionBase {
public:
	ActionNewProject() = delete;
	ActionNewProject(const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "New Project";
	};

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionNewProject)
};

class ActionSave final : public ActionBase {
public:
	ActionSave() = delete;
	ActionSave(const juce::String& name);

	bool doAction() override;
	const juce::String getName() override {
		return "Save";
	};

private:
	ACTION_DATABLOCK{
		const juce::String name;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSave)
};

class ActionLoad final : public ActionBase {
public:
	ActionLoad() = delete;
	ActionLoad(const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "Load";
	};

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionLoad)
};

class ActionInitAudioSource final : public ActionBase {
public:
	ActionInitAudioSource() = delete;
	ActionInitAudioSource(int index,
		double sampleRate, int channels, double length);

	bool doAction() override;
	const juce::String getName() override {
		return "Init Audio Source";
	};

private:
	const int index;
	const double sampleRate;
	const int channels;
	const double length;

	JUCE_LEAK_DETECTOR(ActionInitAudioSource)
};

class ActionInitMidiSource final : public ActionBase {
public:
	ActionInitMidiSource() = delete;
	ActionInitMidiSource(int index);

	bool doAction() override;
	const juce::String getName() override {
		return "Init MIDI Source";
	};

private:
	const int index;

	JUCE_LEAK_DETECTOR(ActionInitMidiSource)
};

class ActionLoadAudioSource final : public ActionBase {
public:
	ActionLoadAudioSource() = delete;
	using SourceCallback = std::function<void(uint64_t)>;
	ActionLoadAudioSource(int index,
		const juce::String& path, const SourceCallback& callback = {});

	bool doAction() override;
	const juce::String getName() override {
		return "Load Audio Source";
	};

private:
	const int index;
	const juce::String path;
	const SourceCallback callback;

	JUCE_LEAK_DETECTOR(ActionLoadAudioSource)
};

class ActionLoadMidiSource final : public ActionBase {
public:
	ActionLoadMidiSource() = delete;
	using SourceCallback = std::function<void(uint64_t)>;
	ActionLoadMidiSource(int index,
		const juce::String& path, bool getTempo = false,
		const SourceCallback& callback = {});

	bool doAction() override;
	const juce::String getName() override {
		return "Load MIDI Source";
	};

private:
	const int index;
	const juce::String path;
	const bool getTempo;
	const SourceCallback callback;

	JUCE_LEAK_DETECTOR(ActionLoadMidiSource)
};

class ActionSaveAudioSource final : public ActionBase {
public:
	ActionSaveAudioSource() = delete;
	ActionSaveAudioSource(int index,
		const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "Save Audio Source";
	};

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSaveAudioSource)
};

class ActionSaveMidiSource final : public ActionBase {
public:
	ActionSaveMidiSource() = delete;
	ActionSaveMidiSource(int index,
		const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "Save MIDI Source";
	};

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSaveMidiSource)
};

class ActionSplitSequencerBlock final : public ActionUndoableBase {
public:
	ActionSplitSequencerBlock() = delete;
	ActionSplitSequencerBlock(
		int track, int block, double time);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Split Sequencer Track Block";
	};

private:
	ACTION_DATABLOCK{
		const int track, block;
		const double time;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSplitSequencerBlock)
};

class ActionLoadPluginState final : public ActionUndoableBase {
public:
	ActionLoadPluginState() = delete;
	ActionLoadPluginState(
		quickAPI::PluginHolder plugin, const juce::String& path);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Load Plugin State";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder plugin;
		const juce::String path;

		juce::MemoryBlock oldState;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionLoadPluginState)
};

class ActionSavePluginState final : public ActionBase {
public:
	ActionSavePluginState() = delete;
	ActionSavePluginState(
		quickAPI::PluginHolder plugin, const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "Save Plugin State";
	};

private:
	const quickAPI::PluginHolder plugin;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSavePluginState)
};
