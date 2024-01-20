#pragma once

#include "ActionUndoableBase.h"
#include "ActionUtils.h"

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

class ActionSynthSource final : public ActionBase {
public:
	ActionSynthSource() = delete;
	ActionSynthSource(int index);

	bool doAction() override;
	const juce::String getName() override {
		return "Synth Source";
	};

private:
	const int index;

	JUCE_LEAK_DETECTOR(ActionSynthSource)
};

class ActionCloneSource final : public ActionBase {
public:
	ActionCloneSource() = delete;
	ActionCloneSource(int index);

	bool doAction() override;
	const juce::String getName() override {
		return "Clone Source";
	};

private:
	ACTION_DATABLOCK{
		const int index;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionCloneSource)
};

class ActionReloadSource final : public ActionBase {
public:
	ActionReloadSource() = delete;
	ActionReloadSource(int index, const juce::String& path, bool copy);

	bool doAction() override;
	const juce::String getName() override {
		return "Reload Source";
	};

private:
	ACTION_DATABLOCK{
		const int index;
		const juce::String path;
		const bool copy;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionCloneSource)
};

class ActionSaveSource final : public ActionBase {
public:
	ActionSaveSource() = delete;
	ActionSaveSource(
		int index, const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "Save Source";
	};

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSaveSource)
};

class ActionSaveSourceAsync final : public ActionBase {
public:
	ActionSaveSourceAsync() = delete;
	ActionSaveSourceAsync(
		int index, const juce::String& path);

	bool doAction() override;
	const juce::String getName() override {
		return "Save Source Async";
	};

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSaveSourceAsync)
};

class ActionRenderNow final : public ActionBase {
public:
	ActionRenderNow() = delete;
	ActionRenderNow(
		const juce::String& path, const juce::String& name,
		const juce::String& extension, const juce::Array<int>& tracks);

	bool doAction() override;
	const juce::String getName() override {
		return "Render Now";
	};

private:
	const juce::String path, name, extension;
	const juce::Array<int> tracks;

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
