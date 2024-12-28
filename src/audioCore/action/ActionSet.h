#pragma once

#include "ActionBase.h"
#include "../quickAPI/QuickGet.h"
#include "../Utils.h"

class ActionSetDeviceAudioType final : public ActionBase {
public:
	ActionSetDeviceAudioType() = delete;
	ActionSetDeviceAudioType(const juce::String& type);

	bool doAction() override;
	const juce::String getName() const override {
		return "Set Device Audio Type";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String type;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioType)
};

class ActionSetDeviceAudioInput final : public ActionBase {
public:
	ActionSetDeviceAudioInput() = delete;
	ActionSetDeviceAudioInput(const juce::String& name);

	bool doAction() override;
	const juce::String getName() const override {
		return "Set Device Audio Input";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioInput)
};

class ActionSetDeviceAudioOutput final : public ActionBase {
public:
	ActionSetDeviceAudioOutput() = delete;
	ActionSetDeviceAudioOutput(const juce::String& name);

	bool doAction() override;
	const juce::String getName() const override {
		return "Set Device Audio Output";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioOutput)
};

class ActionSetDeviceAudioSampleRate final : public ActionBase {
public:
	ActionSetDeviceAudioSampleRate() = delete;
	ActionSetDeviceAudioSampleRate(double sampleRate);

	bool doAction() override;
	const juce::String getName() const override {
		return "Set Device Audio Sample Rate";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const double sampleRate;

	JUCE_LEAK_DETECTOR(ActionSetDeviceAudioSampleRate)
};

class ActionSetDeviceAudioBufferSize final : public ActionBase {
public:
	ActionSetDeviceAudioBufferSize() = delete;
	ActionSetDeviceAudioBufferSize(int bufferSize);

	bool doAction() override;
	const juce::String getName() const override {
		return "Set Device Audio Buffer Size";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

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
	const juce::String getName() const override {
		return "Set Device Midi Input";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

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
	const juce::String getName() const override {
		return "Set Device Midi Output";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String name;

	JUCE_LEAK_DETECTOR(ActionSetDeviceMidiOutput)
};

class ActionSetMidiDebuggerMaxNum final : public ActionBase {
public:
	ActionSetMidiDebuggerMaxNum() = delete;
	ActionSetMidiDebuggerMaxNum(int num);

	bool doAction() override;
	const juce::String getName() const override {
		return "Set Midi Debugger Max Num";
	};
	const juce::String getStatusStr() const override;

private:
	const int num;

	JUCE_LEAK_DETECTOR(ActionSetMidiDebuggerMaxNum)
};

class ActionSetTrackGain final : public ActionUndoableBase {
public:
	ActionSetTrackGain() = delete;
	ActionSetTrackGain(
		quickAPI::TrackIndex track, float value);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Track Gain";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTrackGain; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

	juce::UndoableAction* createCoalescedAction(
		juce::UndoableAction* nextAction) override;

private:
	const quickAPI::TrackIndex track;
	const float value;

	float oldValue = 0;

	JUCE_LEAK_DETECTOR(ActionSetTrackGain)
};

class ActionSetTrackPan final : public ActionUndoableBase {
public:
	ActionSetTrackPan() = delete;
	ActionSetTrackPan(
		quickAPI::TrackIndex track, float value);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Track Pan";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTrackPan; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

	juce::UndoableAction* createCoalescedAction(
		juce::UndoableAction* nextAction) override;

private:
	const quickAPI::TrackIndex track;
	const float value;

	float oldValue = 0;

	JUCE_LEAK_DETECTOR(ActionSetTrackPan)
};

class ActionSetTrackFader final : public ActionUndoableBase {
public:
	ActionSetTrackFader() = delete;
	ActionSetTrackFader(
		quickAPI::TrackIndex track, float value);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Track Fader";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTrackFader; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

	juce::UndoableAction* createCoalescedAction(
		juce::UndoableAction* nextAction) override;

private:
	const quickAPI::TrackIndex track;
	const float value;

	float oldValue = 0;

	JUCE_LEAK_DETECTOR(ActionSetTrackFader)
};

class ActionSetEffectBypass final : public ActionUndoableBase {
public:
	ActionSetEffectBypass() = delete;
	ActionSetEffectBypass(
		quickAPI::TrackIndex track, int effect, bool bypass);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Bypass";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectBypass; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const int effect;
	const bool bypass;

	bool oldBypass = false;

	JUCE_LEAK_DETECTOR(ActionSetEffectBypass)
};

class ActionSetInstrBypass final : public ActionUndoableBase {
public:
	ActionSetInstrBypass() = delete;
	ActionSetInstrBypass(
		int instr, bool bypass);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Bypass";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrBypass; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

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
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Midi Channel";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrMidiChannel; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

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
		quickAPI::TrackIndex track, int effect, int channel);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Midi Channel";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectMidiChannel; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const int effect;
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
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Param Value";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrParamValue; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

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
		quickAPI::TrackIndex track, int effect, int param, float value);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Param Value";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectParamValue; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const int effect, param;
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
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Param Connect To CC";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrParamConnectToCC; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

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
		quickAPI::TrackIndex track, int effect, int param, int cc);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Param Connect To CC";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectParamConnectToCC; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const int effect, param;
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
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Midi CC Intercept";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrMidiCCIntercept; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

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
		quickAPI::TrackIndex track, int effect, bool intercept);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Midi CC Intercept";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectMidiCCIntercept; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const int effect;
	const bool intercept;

	bool oldIntercept = 0;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiCCIntercept)
};

class ActionSetEffectBypassByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectBypassByPtr() = delete;
	ActionSetEffectBypassByPtr(
		quickAPI::PluginHolder effect, bool bypass);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Bypass";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectBypassByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder effect;
	const bool bypass;

	bool oldBypass = false;

	JUCE_LEAK_DETECTOR(ActionSetEffectBypassByPtr)
};

class ActionSetInstrBypassByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrBypassByPtr() = delete;
	ActionSetInstrBypassByPtr(
		quickAPI::PluginHolder instr, bool bypass);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Bypass";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrBypassByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder instr;
	const bool bypass;

	bool oldBypass = false;

	JUCE_LEAK_DETECTOR(ActionSetInstrBypassByPtr)
};

class ActionSetInstrMidiChannelByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrMidiChannelByPtr() = delete;
	ActionSetInstrMidiChannelByPtr(
		quickAPI::PluginHolder instr, int channel);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Midi Channel";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrMidiChannelByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder instr;
	const int channel;

	int oldChannel = -1;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiChannelByPtr)
};

class ActionSetEffectMidiChannelByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectMidiChannelByPtr() = delete;
	ActionSetEffectMidiChannelByPtr(
		quickAPI::PluginHolder effect, int channel);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Midi Channel";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectMidiChannelByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder effect;
	const int channel;

	int oldChannel = -1;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiChannelByPtr)
};

class ActionSetInstrMidiCCInterceptByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrMidiCCInterceptByPtr() = delete;
	ActionSetInstrMidiCCInterceptByPtr(
		quickAPI::PluginHolder instr, bool intercept);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Midi CC Intercept";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrMidiCCInterceptByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder instr;
	const bool intercept;

	bool oldIntercept = 0;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiCCInterceptByPtr)
};


class ActionSetEffectMidiCCInterceptByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectMidiCCInterceptByPtr() = delete;
	ActionSetEffectMidiCCInterceptByPtr(
		quickAPI::PluginHolder effect, bool intercept);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Midi CC Intercept";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectMidiCCInterceptByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder effect;
	const bool intercept;

	bool oldIntercept = 0;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiCCInterceptByPtr)
};

class ActionSetInstrMidiOutputByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrMidiOutputByPtr() = delete;
	ActionSetInstrMidiOutputByPtr(
		quickAPI::PluginHolder instr, bool output);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Midi Output";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrMidiOutputByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder instr;
	const bool output;

	bool oldOutput = 0;

	JUCE_LEAK_DETECTOR(ActionSetInstrMidiOutputByPtr)
};

class ActionSetEffectMidiOutputByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectMidiOutputByPtr() = delete;
	ActionSetEffectMidiOutputByPtr(
		quickAPI::PluginHolder effect, bool output);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Midi Output";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectMidiOutputByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder effect;
	const bool output;

	bool oldOutput = 0;

	JUCE_LEAK_DETECTOR(ActionSetEffectMidiOutputByPtr)
};

class ActionSetInstrParamConnectToCCByPtr final : public ActionUndoableBase {
public:
	ActionSetInstrParamConnectToCCByPtr() = delete;
	ActionSetInstrParamConnectToCCByPtr(
		quickAPI::PluginHolder instr, int param, int cc);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instr Param Connect To CC";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrParamConnectToCCByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder instr;
	const int param;
	const int cc;

	int oldParam = -1;
	int oldCC = -1;

	JUCE_LEAK_DETECTOR(ActionSetInstrParamConnectToCCByPtr)
};

class ActionSetEffectParamConnectToCCByPtr final : public ActionUndoableBase {
public:
	ActionSetEffectParamConnectToCCByPtr() = delete;
	ActionSetEffectParamConnectToCCByPtr(
		quickAPI::PluginHolder effect, int param, int cc);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Param Connect To CC";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectParamConnectToCCByPtr; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::PluginHolder effect;
	const int param;
	const int cc;

	int oldParam = -1;
	int oldCC = -1;

	JUCE_LEAK_DETECTOR(ActionSetEffectParamConnectToCCByPtr)
};

class ActionSetTrackName final : public ActionUndoableBase {
public:
	ActionSetTrackName() = delete;
	ActionSetTrackName(
		quickAPI::TrackIndex track, const juce::String& name);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Track Name";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTrackName; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const juce::String name;

	juce::String oldName;

	JUCE_LEAK_DETECTOR(ActionSetTrackName)
};

class ActionSetTrackColor final : public ActionUndoableBase {
public:
	ActionSetTrackColor() = delete;
	ActionSetTrackColor(
		quickAPI::TrackIndex track, const juce::Colour& color);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Track Color";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTrackColor; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const juce::Colour color;

	juce::Colour oldColor;

	JUCE_LEAK_DETECTOR(ActionSetTrackColor)
};

class ActionSetEffectIndex final : public ActionUndoableBase {
public:
	ActionSetEffectIndex() = delete;
	ActionSetEffectIndex(
		quickAPI::TrackIndex track, int oldIndex, int newIndex);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect Index";
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffectIndex; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const int oldIndex, newIndex;

	JUCE_LEAK_DETECTOR(ActionSetEffectIndex)
};

class ActionSetInstrOffline final : public ActionUndoableBase {
public:
	ActionSetInstrOffline() = delete;
	ActionSetInstrOffline(
		int instr, bool offline);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Instrument Offline";
	};
	ActionType getActionType() const override { return ActionType::ActionSetInstrOffline; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int instr;
	const bool offline;

	bool oldOffline = false;

	JUCE_LEAK_DETECTOR(ActionSetInstrOffline)
};

class ActionSetTempoTime final : public ActionUndoableBase {
public:
	ActionSetTempoTime() = delete;
	ActionSetTempoTime(
		int index, double time);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Tempo Label Time";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTempoTime; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int index;
	const double time;

	double oldTime = 0;
	int newIndex = -1;

	JUCE_LEAK_DETECTOR(ActionSetTempoTime)
};

class ActionSetTempoTempo final : public ActionUndoableBase {
public:
	ActionSetTempoTempo() = delete;
	ActionSetTempoTempo(
		int index, double tempo);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Tempo Label Tempo";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTempoTempo; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int index;
	const double tempo;

	double oldTempo = 0;

	JUCE_LEAK_DETECTOR(ActionSetTempoTempo)
};

class ActionSetTempoBeat final : public ActionUndoableBase {
public:
	ActionSetTempoBeat() = delete;
	ActionSetTempoBeat(
		int index, int numerator, int denominator);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Tempo Label Beat";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTempoBeat; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int index;
	const int numerator, denominator;

	int oldNumerator = 4, oldDenominator = 4;

	JUCE_LEAK_DETECTOR(ActionSetTempoBeat)
};

class ActionSetTrackMute final : public ActionUndoableBase {
public:
	ActionSetTrackMute() = delete;
	ActionSetTrackMute(
		quickAPI::TrackIndex track, bool mute);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Track Mute";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTrackMute; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const bool mute;

	bool oldMute = 0;

	JUCE_LEAK_DETECTOR(ActionSetTrackMute)
};

class ActionSetTrackSolo final : public ActionUndoableBase {
public:
	ActionSetTrackSolo() = delete;
	ActionSetTrackSolo(
		quickAPI::TrackIndex track, bool solo);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Track Solo";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTrackSolo; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const bool solo;

	bool oldSolo = 0;

	JUCE_LEAK_DETECTOR(ActionSetTrackSolo)
};

class ActionSetTrackRecording final : public ActionUndoableBase {
public:
	ActionSetTrackRecording() = delete;
	ActionSetTrackRecording(
		int track, quickAPI::RecordState recordState);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Track Recording";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTrackRecording; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int track;
	const quickAPI::RecordState recordState;

	quickAPI::RecordState oldRecordState = quickAPI::RecordState::NotRecording;

	JUCE_LEAK_DETECTOR(ActionSetTrackRecording)
};

class ActionSetTrackInputMonitoring final : public ActionUndoableBase {
public:
	ActionSetTrackInputMonitoring() = delete;
	ActionSetTrackInputMonitoring(
		int track, bool inputMonitoring);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Track Input Monitoring";
	};
	ActionType getActionType() const override { return ActionType::ActionSetTrackInputMonitoring; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int track;
	const bool inputMonitoring;

	bool oldInputMonitoring = false;

	JUCE_LEAK_DETECTOR(ActionSetTrackInputMonitoring)
};

class ActionSetEffect final : public ActionUndoableBase {
public:
	ActionSetEffect() = delete;
	ActionSetEffect(
		quickAPI::TrackIndex track, int effect, const juce::String& pid);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Effect";
	};
	int getShieldMask() const override {
		return ShieldRendering | ShieldPluginScan;
	};
	ActionType getActionType() const override { return ActionType::ActionSetEffect; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const quickAPI::TrackIndex track;
	const int effect;
	const juce::String pid;

	juce::MemoryBlock data;

	JUCE_LEAK_DETECTOR(ActionSetEffect)
};

class ActionSetCurrentMIDITrack final : public ActionUndoableBase {
public:
	ActionSetCurrentMIDITrack() = delete;
	ActionSetCurrentMIDITrack(
		int track, int midiTrack);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Current MIDI Track";
	};
	ActionType getActionType() const override { return ActionType::ActionSetCurrentMIDITrack; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int track;
	const int midiTrack;

	int oldMIDITrack = 0;

	JUCE_LEAK_DETECTOR(ActionSetCurrentMIDITrack)
};

class ActionSetSequencerBlockTime final : public ActionUndoableBase {
public:
	using BlockTime = std::tuple<double, double, double>;
	ActionSetSequencerBlockTime() = delete;
	ActionSetSequencerBlockTime(
		int track, int index, const BlockTime& time);

	bool doAction() override;
	bool undoAction() override;
	const juce::String getName() const override {
		return "Set Sequencer Block Time";
	};
	ActionType getActionType() const override { return ActionType::ActionSetSequencerBlockTime; };
	const juce::String getStatusStr() const override;
	void getRecoveryData(juce::MemoryOutputStream& stream) override;

private:
	const int track, index;
	const BlockTime time;

	int newIndex = -1;
	BlockTime oldTime{};

	JUCE_LEAK_DETECTOR(ActionSetSequencerBlockTime)
};

class ActionSetPlayPosition final : public ActionBase {
public:
	ActionSetPlayPosition() = delete;
	ActionSetPlayPosition(double pos);

	bool doAction() override;
	const juce::String getName() const override {
		return "Set Play Position";
	};
	int getShieldMask() const override {
		return ShieldRendering;
	};
	const juce::String getStatusStr() const override;

private:
	const double pos;

	JUCE_LEAK_DETECTOR(ActionSetPlayPosition)
};

class ActionSetReturnToStart final : public ActionBase {
public:
	ActionSetReturnToStart() = delete;
	ActionSetReturnToStart(bool returnToStart);

	bool doAction() override;
	const juce::String getName() const override {
		return "Set Return To Start";
	};
	const juce::String getStatusStr() const override;

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
	const juce::String getName() const override {
		return "Set Audio Save Bits Per Sample";
	};
	int getShieldMask() const override {
		return ShieldSourceIO;
	};
	const juce::String getStatusStr() const override;

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
	const juce::String getName() const override {
		return "Set Audio Save Meta Data";
	};
	int getShieldMask() const override {
		return ShieldSourceIO;
	};
	const juce::String getStatusStr() const override;

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
	const juce::String getName() const override {
		return "Set Audio Save Quality Option Index";
	};
	int getShieldMask() const override {
		return ShieldSourceIO;
	};
	const juce::String getStatusStr() const override;

private:
	const juce::String format;
	const int quality;

	JUCE_LEAK_DETECTOR(ActionSetAudioSaveQualityOptionIndex)
};
