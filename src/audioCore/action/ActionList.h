#pragma once

#include "ActionUndoableBase.h"

class ActionListDeviceAudio final : public ActionBase {
public:
	ActionListDeviceAudio();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionListDeviceAudio)
};

class ActionListDeviceMidi final : public ActionBase {
public:
	ActionListDeviceMidi();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionListDeviceMidi)
};

class ActionListPluginBlackList final : public ActionBase {
public:
	ActionListPluginBlackList();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionListPluginBlackList)
};

class ActionListPluginSearchPath final : public ActionBase {
public:
	ActionListPluginSearchPath();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionListPluginSearchPath)
};

class ActionListPlugin final : public ActionBase {
public:
	ActionListPlugin();

	bool doAction() override;

private:
	JUCE_LEAK_DETECTOR(ActionListPlugin)
};

class ActionListInstrParam final : public ActionBase {
public:
	ActionListInstrParam() = delete;
	ActionListInstrParam(int instr);

	bool doAction() override;

private:
	const int instr;

	JUCE_LEAK_DETECTOR(ActionListInstrParam)
};

class ActionListEffectParam final : public ActionBase {
public:
	ActionListEffectParam() = delete;
	ActionListEffectParam(
		int track, int effect);

	bool doAction() override;

private:
	const int track, effect;

	JUCE_LEAK_DETECTOR(ActionListEffectParam)
};
