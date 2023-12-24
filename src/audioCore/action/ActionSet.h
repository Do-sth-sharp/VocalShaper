#pragma once

#include "ActionUndoableBase.h"
#include "ActionUtils.h"
#include "../Utils.h"

class ActionSetDeviceAudioType final : public ActionBase {
public:
	ActionSetDeviceAudioType() = delete;
	ActionSetDeviceAudioType(const juce::String& type);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Device Audio Type";
	};

private:
	const juce::String type;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioType)
};

class ActionSetDeviceAudioInput final : public ActionBase {
public:
	ActionSetDeviceAudioInput() = delete;
	ActionSetDeviceAudioInput(const juce::String& name);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Device Audio Input";
	};

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioInput)
};

class ActionSetDeviceAudioOutput final : public ActionBase {
public:
	ActionSetDeviceAudioOutput() = delete;
	ActionSetDeviceAudioOutput(const juce::String& name);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Device Audio Output";
	};

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioOutput)
};

class ActionSetDeviceAudioSampleRate final : public ActionBase {
public:
	ActionSetDeviceAudioSampleRate() = delete;
	ActionSetDeviceAudioSampleRate(double sampleRate);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Device Audio Sample Rate";
	};

private:
	const double sampleRate;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioSampleRate)
};

class ActionSetDeviceAudioBufferSize final : public ActionBase {
public:
	ActionSetDeviceAudioBufferSize() = delete;
	ActionSetDeviceAudioBufferSize(int bufferSize);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Device Audio Buffer Size";
	};

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
	const juce::String getName() override {
		return "Set Device Midi Input";
	};

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
	const juce::String getName() override {
		return "Set Device Midi Output";
	};

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSetDeviceMidiOutput)
};

class ActionSetMidiDebuggerMaxNum final : public ActionBase {
public:
	ActionSetMidiDebuggerMaxNum() = delete;
	ActionSetMidiDebuggerMaxNum(int num);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Midi Debugger Max Num";
	};

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
	const juce::String getName() override {
		return "Set Mixer Track Gain";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const float value;

		float oldValue = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackGain)
};

class ActionSetMixerTrackPan final : public ActionUndoableBase {
public:
	ActionSetMixerTrackPan() = delete;
	ActionSetMixerTrackPan(
		int track, float value);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Mixer Track Pan";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const float value;

		float oldValue = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackPan)
};

class ActionSetMixerTrackSlider final : public ActionUndoableBase {
public:
	ActionSetMixerTrackSlider() = delete;
	ActionSetMixerTrackSlider(
		int track, float value);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Mixer Track Slider";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const float value;

		float oldValue = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackSlider)
};

class ActionSetEffectWindow final : public ActionBase {
public:
	ActionSetEffectWindow() = delete;
	ActionSetEffectWindow(
		int track, int effect, bool visible);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Effect Window";
	};

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
	const juce::String getName() override {
		return "Set Effect Bypass";
	};

private:
	ACTION_DATABLOCK{
		const int track, effect;
		const bool bypass;

		bool oldBypass = false;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectBypass)
};

class ActionSetInstrWindow final : public ActionBase {
public:
	ActionSetInstrWindow() = delete;
	ActionSetInstrWindow(
		int instr, bool visible);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Instr Window";
	};

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
	const juce::String getName() override {
		return "Set Instr Bypass";
	};

private:
	ACTION_DATABLOCK{
		const int instr;
		const bool bypass;

		bool oldBypass = false;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrBypass)
};

class ActionSetInstrMidiChannel final : public ActionUndoableBase {
public:
	ActionSetInstrMidiChannel() = delete;
	ActionSetInstrMidiChannel(
		int instr, int channel);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instr Midi Channel";
	};

private:
	ACTION_DATABLOCK{
		const int instr;
		const int channel;

		int oldChannel = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiChannel)
};

class ActionSetEffectMidiChannel final : public ActionUndoableBase {
public:
	ActionSetEffectMidiChannel() = delete;
	ActionSetEffectMidiChannel(
		int track, int effect, int channel);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Midi Channel";
	};

private:
	ACTION_DATABLOCK{
		const int track, effect;
		const int channel;

		int oldChannel = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiChannel)
};

class ActionSetInstrParamValue final : public ActionUndoableBase {
public:
	ActionSetInstrParamValue() = delete;
	ActionSetInstrParamValue(
		int instr, int param, float value);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instr Param Value";
	};

private:
	ACTION_DATABLOCK{
		const int instr, param;
		const float value;

		float oldValue = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrParamValue)
};

class ActionSetEffectParamValue final : public ActionUndoableBase {
public:
	ActionSetEffectParamValue() = delete;
	ActionSetEffectParamValue(
		int track, int effect, int param, float value);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Param Value";
	};

private:
	ACTION_DATABLOCK{
		const int track, effect, param;
		const float value;

		float oldValue = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectParamValue)
};

class ActionSetInstrParamConnectToCC final : public ActionUndoableBase {
public:
	ActionSetInstrParamConnectToCC() = delete;
	ActionSetInstrParamConnectToCC(
		int instr, int param, int cc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instr Param Connect To CC";
	};

private:
	ACTION_DATABLOCK{
		const int instr, param;
		const int cc;

		int oldParam = -1;
		int oldCC = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrParamConnectToCC)
};

class ActionSetEffectParamConnectToCC final : public ActionUndoableBase {
public:
	ActionSetEffectParamConnectToCC() = delete;
	ActionSetEffectParamConnectToCC(
		int track, int effect, int param, int cc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Param Connect To CC";
	};

private:
	ACTION_DATABLOCK{
		const int track, effect, param;
		const int cc;

		int oldParam = -1;
		int oldCC = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectParamConnectToCC)
};

class ActionSetInstrMidiCCIntercept final : public ActionUndoableBase {
public:
	ActionSetInstrMidiCCIntercept() = delete;
	ActionSetInstrMidiCCIntercept(
		int instr, bool intercept);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instr Midi CC Intercept";
	};

private:
	ACTION_DATABLOCK{
		const int instr;
		const bool intercept;

		bool oldIntercept = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiCCIntercept)
};

class ActionSetEffectMidiCCIntercept final : public ActionUndoableBase {
public:
	ActionSetEffectMidiCCIntercept() = delete;
	ActionSetEffectMidiCCIntercept(
		int track, int effect, bool intercept);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Midi CC Intercept";
	};

private:
	ACTION_DATABLOCK{
		const int track, effect;
		const bool intercept;

		bool oldIntercept = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiCCIntercept)
};

class ActionSetSequencerTrackBypass final : public ActionUndoableBase {
public:
	ActionSetSequencerTrackBypass() = delete;
	ActionSetSequencerTrackBypass(
		int track, bool bypass);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Sequencer Track Bypass";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const bool bypass;

		bool oldBypass = false;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetSequencerTrackBypass)
};

class ActionSetPlayPosition final : public ActionBase {
public:
	ActionSetPlayPosition() = delete;
	ActionSetPlayPosition(double pos);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Play Position";
	};

private:
	const double pos;

	JUCE_LEAK_DETECTOR(ActionSetPlayPosition)
};

class ActionSetReturnToStart final : public ActionBase {
public:
	ActionSetReturnToStart() = delete;
	ActionSetReturnToStart(bool returnToStart);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Return To Start";
	};

private:
	const bool returnToStart;

	JUCE_LEAK_DETECTOR(ActionSetReturnToStart)
};

class ActionSetSourceSynthesizer final : public ActionBase {
public:
	ActionSetSourceSynthesizer() = delete;
	ActionSetSourceSynthesizer(
		int index, const juce::String& pid);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Source Synthesizer";
	};

private:
	ACTION_DATABLOCK{
		const int index;
		const juce::String pid;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetSourceSynthesizer)
};

class ActionSetAudioSaveBitsPerSample final : public ActionBase {
public:
	ActionSetAudioSaveBitsPerSample() = delete;
	ActionSetAudioSaveBitsPerSample(
		const juce::String& format, int bitPerSample);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Audio Save Bits Per Sample";
	};

private:
	const juce::String format;
	const int bitPerSample;

	JUCE_LEAK_DETECTOR(ActionSetAudioSaveBitsPerSample)
};

class ActionSetAudioSaveMetaData final : public ActionBase {
public:
	ActionSetAudioSaveMetaData() = delete;
	ActionSetAudioSaveMetaData(
		const juce::String& format, const juce::StringPairArray& metaData);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Audio Save Meta Data";
	};

private:
	const juce::String format;
	const juce::StringPairArray metaData;

	JUCE_LEAK_DETECTOR(ActionSetAudioSaveMetaData)
};

class ActionSetAudioSaveQualityOptionIndex final : public ActionBase {
public:
	ActionSetAudioSaveQualityOptionIndex() = delete;
	ActionSetAudioSaveQualityOptionIndex(
		const juce::String& format, int quality);

	bool doAction() override;
	const juce::String getName() override {
		return "Set Audio Save Quality Option Index";
	};

private:
	const juce::String format;
	const int quality;

	JUCE_LEAK_DETECTOR(ActionSetAudioSaveQualityOptionIndex)
};
