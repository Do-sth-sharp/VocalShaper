#pragma once

#include "ActionUndoableBase.h"

class ActionClearPlugin final : public ActionBase {
public:
	ActionClearPlugin();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionClearPlugin)
};

class ActionSearchPlugin final : public ActionBase {
public:
	ActionSearchPlugin();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionSearchPlugin)
};

class ActionPlay final : public ActionBase {
public:
	ActionPlay();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionPlay)
};

class ActionPause final : public ActionBase {
public:
	ActionPause();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionPause)
};

class ActionStop final : public ActionBase {
public:
	ActionStop();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionStop)
};

class ActionRewind final : public ActionBase {
public:
	ActionRewind();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionRewind)
};

class ActionStartRecord final : public ActionBase {
public:
	ActionStartRecord();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionStartRecord)
};

class ActionStopRecord final : public ActionBase {
public:
	ActionStopRecord();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionStopRecord)
};

class ActionSynthSource final : public ActionBase {
public:
	ActionSynthSource() = delete;
	ActionSynthSource(int index);

	bool doAction() override;

private:
	const int index;

	JUCE_LEAK_DETECTOR(ActionSynthSource)
};

class ActionCloneSource final : public ActionBase {
public:
	ActionCloneSource() = delete;
	ActionCloneSource(int index);

	bool doAction() override;

private:
	const int index;

	JUCE_LEAK_DETECTOR(ActionCloneSource)
};

class ActionSaveSource final : public ActionBase {
public:
	ActionSaveSource() = delete;
	ActionSaveSource(
		int index, const juce::String& path);

	bool doAction() override;

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

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionSaveSourceAsync)
};

class ActionExportSource final : public ActionBase {
public:
	ActionExportSource() = delete;
	ActionExportSource(
		int index, const juce::String& path);

	bool doAction() override;

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionExportSource)
};

class ActionExportSourceAsync final : public ActionBase {
public:
	ActionExportSourceAsync() = delete;
	ActionExportSourceAsync(
		int index, const juce::String& path);

	bool doAction() override;

private:
	const int index;
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionExportSourceAsync)
};

class ActionRenderNow final : public ActionBase {
public:
	ActionRenderNow() = delete;
	ActionRenderNow(
		const juce::String& path, const juce::String& name,
		const juce::String& extension, const juce::Array<int>& tracks);

	bool doAction() override;

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

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionNewProject)
};

class ActionSave final : public ActionBase {
public:
	ActionSave() = delete;
	ActionSave(const juce::String& name);

	bool doAction() override;

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSave)
};

class ActionLoad final : public ActionBase {
public:
	ActionLoad() = delete;
	ActionLoad(const juce::String& path);

	bool doAction() override;

private:
	const juce::String path;

	JUCE_LEAK_DETECTOR(ActionLoad)
};
