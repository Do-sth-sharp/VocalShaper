#pragma once

#include "ActionUndoableBase.h"
#include "../Utils.h"

class ActionSetDeviceAudioType final : public ActionBase {
public:
	ActionSetDeviceAudioType() = delete;
	ActionSetDeviceAudioType(const juce::String& type);

	bool doAction() override;

private:
	const juce::String type;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioType)
};

class ActionSetDeviceAudioInput final : public ActionBase {
public:
	ActionSetDeviceAudioInput() = delete;
	ActionSetDeviceAudioInput(const juce::String& name);

	bool doAction() override;

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioInput)
};

class ActionSetDeviceAudioOutput final : public ActionBase {
public:
	ActionSetDeviceAudioOutput() = delete;
	ActionSetDeviceAudioOutput(const juce::String& name);

	bool doAction() override;

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioOutput)
};

class ActionSetDeviceAudioSampleRate final : public ActionBase {
public:
	ActionSetDeviceAudioSampleRate() = delete;
	ActionSetDeviceAudioSampleRate(double sampleRate);

	bool doAction() override;

private:
	const double sampleRate;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioSampleRate)
};

class ActionSetDeviceAudioBufferSize final : public ActionBase {
public:
	ActionSetDeviceAudioBufferSize() = delete;
	ActionSetDeviceAudioBufferSize(int bufferSize);

	bool doAction() override;

private:
	const int bufferSize;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioBufferSize)
};

class ActionSetDeviceMidiInput final : public ActionBase {
public:
	ActionSetDeviceMidiInput() = delete;
	ActionSetDeviceMidiInput(
		const juce::String& name, bool enabled);

	bool doAction() override;

private:
	const juce::String name;
	const bool enabled;

	JUCE_LEAK_DETECTOR(ActionSetDeviceMidiInput)
};

class ActionSetDeviceMidiOutput final : public ActionBase {
public:
	ActionSetDeviceMidiOutput() = delete;
	ActionSetDeviceMidiOutput(const juce::String& name);

	bool doAction() override;

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSetDeviceMidiOutput)
};

class ActionSetMidiDebuggerMaxNum final : public ActionBase {
public:
	ActionSetMidiDebuggerMaxNum() = delete;
	ActionSetMidiDebuggerMaxNum(int num);

	bool doAction() override;

private:
	const int num;

	JUCE_LEAK_DETECTOR(ActionSetMidiDebuggerMaxNum)
};

class ActionSetMixerTrackGain final : public ActionUndoableBase {
public:
	ActionSetMixerTrackGain() = delete;
	ActionSetMixerTrackGain(
		int track, float value);

	bool doAction() override;
	bool undo() override;

private:
	const int track;
	const float value;

	float oldValue = 0;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackGain)
};

class ActionSetMixerTrackPan final : public ActionUndoableBase {
public:
	ActionSetMixerTrackPan() = delete;
	ActionSetMixerTrackPan(
		int track, float value);

	bool doAction() override;
	bool undo() override;

private:
	const int track;
	const float value;

	float oldValue = 0;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackPan)
};

class ActionSetMixerTrackSlider final : public ActionUndoableBase {
public:
	ActionSetMixerTrackSlider() = delete;
	ActionSetMixerTrackSlider(
		int track, float value);

	bool doAction() override;
	bool undo() override;

private:
	const int track;
	const float value;

	float oldValue = 0;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackSlider)
};

class ActionSetEffectWindow final : public ActionBase {
public:
	ActionSetEffectWindow() = delete;
	ActionSetEffectWindow(
		int track, int effect, bool visible);

	bool doAction() override;

private:
	const int track, effect;
	const bool visible;

	JUCE_LEAK_DETECTOR(ActionSetEffectWindow)
};

class ActionSetEffectBypass final : public ActionUndoableBase {
public:
	ActionSetEffectBypass() = delete;
	ActionSetEffectBypass(
		int track, int effect, bool bypass);

	bool doAction() override;
	bool undo() override;

private:
	const int track, effect;
	const bool bypass;

	bool oldBypass = false;

	JUCE_LEAK_DETECTOR(ActionSetEffectBypass)
};

class ActionSetInstrWindow final : public ActionBase {
public:
	ActionSetInstrWindow() = delete;
	ActionSetInstrWindow(
		int instr, bool visible);

	bool doAction() override;

private:
	const int instr;
	const bool visible;

	JUCE_LEAK_DETECTOR(ActionSetInstrWindow)
};

class ActionSetInstrBypass final : public ActionUndoableBase {
public:
	ActionSetInstrBypass() = delete;
	ActionSetInstrBypass(
		int instr, bool bypass);

	bool doAction() override;
	bool undo() override;

private:
	const int instr;
	const bool bypass;

	bool oldBypass = false;

	JUCE_LEAK_DETECTOR(ActionSetInstrBypass)
};

class ActionSetInstrMidiChannel final : public ActionUndoableBase {
public:
	ActionSetInstrMidiChannel() = delete;
	ActionSetInstrMidiChannel(
		int instr, int channel);

	bool doAction() override;
	bool undo() override;

private:
	const int instr;
	const int channel;

	int oldChannel = -1;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiChannel)
};

class ActionSetEffectMidiChannel final : public ActionUndoableBase {
public:
	ActionSetEffectMidiChannel() = delete;
	ActionSetEffectMidiChannel(
		int track, int effect, int channel);

	bool doAction() override;
	bool undo() override;

private:
	const int track, effect;
	const int channel;

	int oldChannel = -1;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiChannel)
};

class ActionSetInstrParamValue final : public ActionUndoableBase {
public:
	ActionSetInstrParamValue() = delete;
	ActionSetInstrParamValue(
		int instr, int param, float value);

	bool doAction() override;
	bool undo() override;

private:
	const int instr, param;
	const float value;

	float oldValue = 0;

	JUCE_LEAK_DETECTOR(ActionSetInstrParamValue)
};

class ActionSetEffectParamValue final : public ActionUndoableBase {
public:
	ActionSetEffectParamValue() = delete;
	ActionSetEffectParamValue(
		int track, int effect, int param, float value);

	bool doAction() override;
	bool undo() override;

private:
	const int track, effect, param;
	const float value;

	float oldValue = 0;

	JUCE_LEAK_DETECTOR(ActionSetEffectParamValue)
};

class ActionSetInstrParamConnectToCC final : public ActionUndoableBase {
public:
	ActionSetInstrParamConnectToCC() = delete;
	ActionSetInstrParamConnectToCC(
		int instr, int param, int cc);

	bool doAction() override;
	bool undo() override;

private:
	const int instr, param;
	const int cc;

	int oldParam = -1;
	int oldCC = -1;

	JUCE_LEAK_DETECTOR(ActionSetInstrParamConnectToCC)
};

class ActionSetEffectParamConnectToCC final : public ActionUndoableBase {
public:
	ActionSetEffectParamConnectToCC() = delete;
	ActionSetEffectParamConnectToCC(
		int track, int effect, int param, int cc);

	bool doAction() override;
	bool undo() override;

private:
	const int track, effect, param;
	const int cc;

	int oldParam = -1;
	int oldCC = -1;

	JUCE_LEAK_DETECTOR(ActionSetEffectParamConnectToCC)
};

class ActionSetInstrMidiCCIntercept final : public ActionUndoableBase {
public:
	ActionSetInstrMidiCCIntercept() = delete;
	ActionSetInstrMidiCCIntercept(
		int instr, bool intercept);

	bool doAction() override;
	bool undo() override;

private:
	const int instr;
	const bool intercept;

	bool oldIntercept = 0;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiCCIntercept)
};

class ActionSetEffectMidiCCIntercept final : public ActionUndoableBase {
public:
	ActionSetEffectMidiCCIntercept() = delete;
	ActionSetEffectMidiCCIntercept(
		int track, int effect, bool intercept);

	bool doAction() override;
	bool undo() override;

private:
	const int track, effect;
	const bool intercept;

	bool oldIntercept = 0;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiCCIntercept)
};
