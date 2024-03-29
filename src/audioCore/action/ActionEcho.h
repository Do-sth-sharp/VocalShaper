﻿#pragma once

#include "ActionUndoableBase.h"

class ActionEchoDeviceAudio final : public ActionBase {
public:
	ActionEchoDeviceAudio();

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Device Audio";
	};

private:
	JUCE_LEAK_DETECTOR(ActionEchoDeviceAudio)
};

class ActionEchoDeviceMidi final : public ActionBase {
public:
	ActionEchoDeviceMidi();

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Device Midi";
	};

private:
	JUCE_LEAK_DETECTOR(ActionEchoDeviceMidi)
};

class ActionEchoMixerInfo final : public ActionBase {
public:
	ActionEchoMixerInfo();

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Mixer Info";
	};

private:
	JUCE_LEAK_DETECTOR(ActionEchoMixerInfo)
};

class ActionEchoMixerTrack final : public ActionBase {
public:
	ActionEchoMixerTrack();

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Mixer Track";
	};

private:
	JUCE_LEAK_DETECTOR(ActionEchoMixerTrack)
};

class ActionEchoMixerTrackInfo final : public ActionBase {
public:
	ActionEchoMixerTrackInfo() = delete;
	ActionEchoMixerTrackInfo(int index);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Mixer Track Info";
	};

private:
	const int index;

	JUCE_LEAK_DETECTOR(ActionEchoMixerTrackInfo)
};

class ActionEchoMixerTrackGain final : public ActionBase {
public:
	ActionEchoMixerTrackGain() = delete;
	ActionEchoMixerTrackGain(int index);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Mixer Track Gain";
	};

private:
	const int index;

	JUCE_LEAK_DETECTOR(ActionEchoMixerTrackGain)
};

class ActionEchoMixerTrackPan final : public ActionBase {
public:
	ActionEchoMixerTrackPan() = delete;
	ActionEchoMixerTrackPan(int index);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Mixer Track Pan";
	};

private:
	const int index;

	JUCE_LEAK_DETECTOR(ActionEchoMixerTrackPan)
};

class ActionEchoMixerTrackSlider final : public ActionBase {
public:
	ActionEchoMixerTrackSlider() = delete;
	ActionEchoMixerTrackSlider(int index);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Mixer Track Slider";
	};

private:
	const int index;

	JUCE_LEAK_DETECTOR(ActionEchoMixerTrackSlider)
};

class ActionEchoInstrParamValue final : public ActionBase {
public:
	ActionEchoInstrParamValue() = delete;
	ActionEchoInstrParamValue(
		int instr, int param);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Instr Param Value";
	};

private:
	const int instr, param;

	JUCE_LEAK_DETECTOR(ActionEchoInstrParamValue)
};

class ActionEchoInstrParamDefaultValue final : public ActionBase {
public:
	ActionEchoInstrParamDefaultValue() = delete;
	ActionEchoInstrParamDefaultValue(
		int instr, int param);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Instr Param Default Value";
	};

private:
	const int instr, param;

	JUCE_LEAK_DETECTOR(ActionEchoInstrParamDefaultValue)
};

class ActionEchoEffectParamValue final : public ActionBase {
public:
	ActionEchoEffectParamValue() = delete;
	ActionEchoEffectParamValue(
		int track, int effect, int param);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Effect Param Value";
	};

private:
	const int track, effect, param;

	JUCE_LEAK_DETECTOR(ActionEchoEffectParamValue)
};

class ActionEchoEffectParamDefaultValue final : public ActionBase {
public:
	ActionEchoEffectParamDefaultValue() = delete;
	ActionEchoEffectParamDefaultValue(
		int track, int effect, int param);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Effect Param Default Value";
	};

private:
	const int track, effect, param;

	JUCE_LEAK_DETECTOR(ActionEchoEffectParamDefaultValue)
};

class ActionEchoInstrParamCC final : public ActionBase {
public:
	ActionEchoInstrParamCC() = delete;
	ActionEchoInstrParamCC(
		int instr, int param);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Instr Param CC";
	};

private:
	const int instr, param;

	JUCE_LEAK_DETECTOR(ActionEchoInstrParamCC)
};

class ActionEchoEffectParamCC final : public ActionBase {
public:
	ActionEchoEffectParamCC() = delete;
	ActionEchoEffectParamCC(
		int track, int effect, int param);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Effect Param CC";
	};

private:
	const int track, effect, param;

	JUCE_LEAK_DETECTOR(ActionEchoEffectParamCC)
};

class ActionEchoInstrCCParam final : public ActionBase {
public:
	ActionEchoInstrCCParam() = delete;
	ActionEchoInstrCCParam(
		int instr, int cc);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Instr CC Param";
	};

private:
	const int instr, cc;

	JUCE_LEAK_DETECTOR(ActionEchoInstrCCParam)
};

class ActionEchoEffectCCParam final : public ActionBase {
public:
	ActionEchoEffectCCParam() = delete;
	ActionEchoEffectCCParam(
		int track, int effect, int cc);

	bool doAction() override;
	const juce::String getName() override {
		return "Echo Effect CC Param";
	};

private:
	const int track, effect, cc;

	JUCE_LEAK_DETECTOR(ActionEchoEffectCCParam)
};
