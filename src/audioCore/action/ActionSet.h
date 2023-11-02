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
