#pragma once

#include "ActionUndoableBase.h"
#include "ActionUtils.h"
#include "../quickAPI/QuickGet.h"
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

	juce::UndoableAction* createCoalescedAction(
		juce::UndoableAction* nextAction) override;

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

	juce::UndoableAction* createCoalescedAction(
		juce::UndoableAction* nextAction) override;

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

	juce::UndoableAction* createCoalescedAction(
		juce::UndoableAction* nextAction) override;

private:
	ACTION_DATABLOCK{
		const int track;
		const float value;

		float oldValue = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackSlider)
};

class ActionSetMixerTrackMute final : public ActionUndoableBase {
public:
	ActionSetMixerTrackMute() = delete;
	ActionSetMixerTrackMute(
		int track, bool mute);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Mixer Track Mute";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const bool mute;

		bool oldMute = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackMute)
};

class ActionSetMixerTrackName final : public ActionUndoableBase {
public:
	ActionSetMixerTrackName() = delete;
	ActionSetMixerTrackName(
		int track, const juce::String& name);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Mixer Track Name";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const juce::String name;

		juce::String oldName;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackName)
};

class ActionSetMixerTrackColor final : public ActionUndoableBase {
public:
	ActionSetMixerTrackColor() = delete;
	ActionSetMixerTrackColor(
		int track, const juce::Colour& color);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Mixer Track Color";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const juce::Colour color;

		juce::Colour oldColor;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetMixerTrackColor)
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

class ActionSetEffectBypassByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectBypassByPtr() = delete;
	ActionSetEffectBypassByPtr(
		quickAPI::PluginHolder effect, bool bypass);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Bypass";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder effect;
		const bool bypass;

		bool oldBypass = false;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectBypassByPtr)
};

class ActionSetSequencerTrackMute final : public ActionUndoableBase {
public:
	ActionSetSequencerTrackMute() = delete;
	ActionSetSequencerTrackMute(
		int track, bool mute);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Sequencer Track Mute";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const bool mute;

		bool oldMute = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetSequencerTrackMute)
};

class ActionSetSequencerTrackName final : public ActionUndoableBase {
public:
	ActionSetSequencerTrackName() = delete;
	ActionSetSequencerTrackName(
		int track, const juce::String& name);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Sequencer Track Name";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const juce::String name;

		juce::String oldName;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetSequencerTrackName)
};

class ActionSetSequencerTrackColor final : public ActionUndoableBase {
public:
	ActionSetSequencerTrackColor() = delete;
	ActionSetSequencerTrackColor(
		int track, const juce::Colour& color);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Sequencer Track Color";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const juce::Colour color;

		juce::Colour oldColor;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetSequencerTrackColor)
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

class ActionSetInstrBypassByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrBypassByPtr() = delete;
	ActionSetInstrBypassByPtr(
		quickAPI::PluginHolder instr, bool bypass);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instr Bypass";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder instr;
		const bool bypass;

		bool oldBypass = false;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrBypassByPtr)
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

class ActionSetInstrMidiChannelByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrMidiChannelByPtr() = delete;
	ActionSetInstrMidiChannelByPtr(
		quickAPI::PluginHolder instr, int channel);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instr Midi Channel";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder instr;
		const int channel;

		int oldChannel = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiChannelByPtr)
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

class ActionSetEffectMidiChannelByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectMidiChannelByPtr() = delete;
	ActionSetEffectMidiChannelByPtr(
		quickAPI::PluginHolder effect, int channel);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Midi Channel";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder effect;
		const int channel;

		int oldChannel = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiChannelByPtr)
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

class ActionSetEffectIndex final : public ActionUndoableBase {
public:
	ActionSetEffectIndex() = delete;
	ActionSetEffectIndex(
		int track, int oldIndex, int newIndex);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Index";
	};

private:
	ACTION_DATABLOCK{
		const int track, oldIndex, newIndex;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectIndex)
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

class ActionSetInstrParamConnectToCCByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrParamConnectToCCByPtr() = delete;
	ActionSetInstrParamConnectToCCByPtr(
		quickAPI::PluginHolder instr, int param, int cc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instr Param Connect To CC";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder instr;
		const int param;
		const int cc;

		int oldParam = -1;
		int oldCC = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrParamConnectToCCByPtr)
};

class ActionSetEffectParamConnectToCCByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectParamConnectToCCByPtr() = delete;
	ActionSetEffectParamConnectToCCByPtr(
		quickAPI::PluginHolder effect, int param, int cc);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Param Connect To CC";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder effect;
		const int param;
		const int cc;

		int oldParam = -1;
		int oldCC = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectParamConnectToCCByPtr)
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

class ActionSetInstrMidiCCInterceptByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrMidiCCInterceptByPtr() = delete;
	ActionSetInstrMidiCCInterceptByPtr(
		quickAPI::PluginHolder instr, bool intercept);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instr Midi CC Intercept";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder instr;
		const bool intercept;

		bool oldIntercept = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiCCInterceptByPtr)
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

class ActionSetEffectMidiCCInterceptByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectMidiCCInterceptByPtr() = delete;
	ActionSetEffectMidiCCInterceptByPtr(
		quickAPI::PluginHolder effect, bool intercept);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Midi CC Intercept";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder effect;
		const bool intercept;

		bool oldIntercept = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiCCInterceptByPtr)
};

class ActionSetInstrMidiOutputByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrMidiOutputByPtr() = delete;
	ActionSetInstrMidiOutputByPtr(
		quickAPI::PluginHolder instr, bool output);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instr Midi Output";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder instr;
		const bool output;

		bool oldOutput = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiOutputByPtr)
};

class ActionSetEffectMidiOutputByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectMidiOutputByPtr() = delete;
	ActionSetEffectMidiOutputByPtr(
		quickAPI::PluginHolder effect, bool output);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect Midi Output";
	};

private:
	ACTION_DATABLOCK{
		const quickAPI::PluginHolder effect;
		const bool output;

		bool oldOutput = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiOutputByPtr)
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

class ActionSetSequencerTrackRecording final : public ActionUndoableBase {
public:
	ActionSetSequencerTrackRecording() = delete;
	ActionSetSequencerTrackRecording(
		int track, bool recording);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Sequencer Track Recording";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const bool recording;

		bool oldRecording = false;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetSequencerTrackRecording)
};

class ActionSetInstrOffline final : public ActionUndoableBase {
public:
	ActionSetInstrOffline() = delete;
	ActionSetInstrOffline(
		int instr, bool offline);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Instrument Offline";
	};

private:
	ACTION_DATABLOCK{
		const int instr;
		const bool offline;

		bool oldOffline = false;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetInstrOffline)
};

class ActionSetTempoTime final : public ActionUndoableBase {
public:
	ActionSetTempoTime() = delete;
	ActionSetTempoTime(
		int index, double time);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Tempo Label Time";
	};

private:
	ACTION_DATABLOCK{
		const int index;
		const double time;

		double oldTime = 0;
		int newIndex = -1;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetTempoTime)
};

class ActionSetTempoTempo final : public ActionUndoableBase {
public:
	ActionSetTempoTempo() = delete;
	ActionSetTempoTempo(
		int index, double tempo);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Tempo Label Tempo";
	};

private:
	ACTION_DATABLOCK{
		const int index;
		const double tempo;

		double oldTempo = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetTempoTempo)
};

class ActionSetTempoBeat final : public ActionUndoableBase {
public:
	ActionSetTempoBeat() = delete;
	ActionSetTempoBeat(
		int index, int numerator, int denominator);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Tempo Label Beat";
	};

private:
	ACTION_DATABLOCK{
		const int index;
		const int numerator, denominator;

		int oldNumerator = 4, oldDenominator = 4;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetTempoBeat)
};

class ActionSetEffect final : public ActionUndoableBase {
public:
	ActionSetEffect() = delete;
	ActionSetEffect(
		int track, int effect, const juce::String& pid);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Effect";
	};

private:
	ACTION_DATABLOCK{
		const int track, effect;
		const juce::String pid;

		juce::MemoryBlock data;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetEffect)
};

class ActionSetSequencerMIDITrack final : public ActionUndoableBase {
public:
	ActionSetSequencerMIDITrack() = delete;
	ActionSetSequencerMIDITrack(
		int track, int midiTrack);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Sequencer MIDI Track";
	};

private:
	ACTION_DATABLOCK{
		const int track;
		const int midiTrack;

		int oldMIDITrack = 0;
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetSequencerMIDITrack)
};

class ActionSetSequencerBlockTime final : public ActionUndoableBase {
public:
	using BlockTime = std::tuple<double, double, double>;
	ActionSetSequencerBlockTime() = delete;
	ActionSetSequencerBlockTime(
		int track, int index, const BlockTime& time);

	bool doAction() override;
	bool undo() override;
	const juce::String getName() override {
		return "Set Sequencer Block Time";
	};

private:
	ACTION_DATABLOCK{
		const int track, index;
		const BlockTime time;

		int newIndex = -1;
		BlockTime oldTime{};
	} ACTION_DB;

	JUCE_LEAK_DETECTOR(ActionSetSequencerBlockTime)
};