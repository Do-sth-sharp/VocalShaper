#pragma once

#include "ActionUndoableBase.h"

class ActionListDeviceAudio final : public ActionBase {
public:
	ActionListDeviceAudio();

	bool doAction() override;
	const juce::String getName() override {
		return "List Device Audio";
	};

private:
	JUCE_LEAK_DETECTOR(ActionListDeviceAudio)
};

class ActionListDeviceMidi final : public ActionBase {
public:
	ActionListDeviceMidi();

	bool doAction() override;
	const juce::String getName() override {
		return "List Device Midi";
	};

private:
	JUCE_LEAK_DETECTOR(ActionListDeviceMidi)
};

class ActionListPluginBlackList final : public ActionBase {
public:
	ActionListPluginBlackList();

	bool doAction() override;
	const juce::String getName() override {
		return "List Plugin Black List";
	};

private:
	JUCE_LEAK_DETECTOR(ActionListPluginBlackList)
};

class ActionListPluginSearchPath final : public ActionBase {
public:
	ActionListPluginSearchPath();

	bool doAction() override;
	const juce::String getName() override {
		return "List Plugin Search Path";
	};

private:
	JUCE_LEAK_DETECTOR(ActionListPluginSearchPath)
};

class ActionListPlugin final : public ActionBase {
public:
	ActionListPlugin();

	bool doAction() override;
	const juce::String getName() override {
		return "List Plugin";
	};

private:
	JUCE_LEAK_DETECTOR(ActionListPlugin)
};

class ActionListInstrParam final : public ActionBase {
public:
	ActionListInstrParam() = delete;
	ActionListInstrParam(int instr);

	bool doAction() override;
	const juce::String getName() override {
		return "List Instr Param";
	};

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
	const juce::String getName() override {
		return "List Effect Param";
	};

private:
	const int track, effect;

	JUCE_LEAK_DETECTOR(ActionListEffectParam)
};
