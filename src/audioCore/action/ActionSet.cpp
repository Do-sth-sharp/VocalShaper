#include "ActionSet.h"

#include "../AudioCore.h"
#include "../misc/Device.h"

ActionSetDeviceAudioType::ActionSetDeviceAudioType(const juce::String& type)
	: type(type) {}

bool ActionSetDeviceAudioType::doAction() {
	Device::getInstance()->setCurrentAudioDeviceType(this->type);

	juce::String result;
	result += "Current Audio Device Type: " + Device::getInstance()->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + Device::getInstance()->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + Device::getInstance()->getAudioOutputDeviceName() + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceAudioInput::ActionSetDeviceAudioInput(const juce::String& name)
	: name(name) {}

bool ActionSetDeviceAudioInput::doAction() {
	juce::String result;
	auto err = Device::getInstance()->setAudioInputDevice(this->name);
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Device Type: " + Device::getInstance()->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + Device::getInstance()->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + Device::getInstance()->getAudioOutputDeviceName() + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceAudioOutput::ActionSetDeviceAudioOutput(const juce::String& name)
	: name(name) {}

bool ActionSetDeviceAudioOutput::doAction() {
	juce::String result;
	auto err = Device::getInstance()->setAudioOutputDevice(this->name);
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Device Type: " + Device::getInstance()->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + Device::getInstance()->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + Device::getInstance()->getAudioOutputDeviceName() + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceAudioSampleRate::ActionSetDeviceAudioSampleRate(double sampleRate)
	: sampleRate(sampleRate) {}

bool ActionSetDeviceAudioSampleRate::doAction() {
	juce::String result;
	auto err = Device::getInstance()->setAudioSampleRate(this->sampleRate);
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Sample Rate: " + juce::String(Device::getInstance()->getAudioSampleRate()) + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceAudioBufferSize::ActionSetDeviceAudioBufferSize(int bufferSize)
	: bufferSize(bufferSize) {}

bool ActionSetDeviceAudioBufferSize::doAction() {
	juce::String result;
	auto err = Device::getInstance()->setAudioBufferSize(this->bufferSize);
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Buffer Size: " + juce::String(Device::getInstance()->getAudioBufferSize()) + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceMidiInput::ActionSetDeviceMidiInput(
	const juce::String& name, bool enabled)
	: name(name), enabled(enabled) {}

bool ActionSetDeviceMidiInput::doAction() {
	juce::String result;

	Device::getInstance()->setMIDIInputDeviceEnabled(
		this->name,	this->enabled);

	result += "MIDI Input Device: " + this->name + " - " + (Device::getInstance()->getMIDIInputDeviceEnabled(this->name) ? "ON" : "OFF") + "\n";
	this->output(result);
	return true;
}

ActionSetDeviceMidiOutput::ActionSetDeviceMidiOutput(const juce::String& name)
	: name(name) {}

bool ActionSetDeviceMidiOutput::doAction() {
	juce::String result;

	Device::getInstance()->setMIDIOutputDevice(this->name);
	result += "MIDI Output Device: " + this->name + "\n";
	this->output(result);
	return true;
}

ActionSetMidiDebuggerMaxNum::ActionSetMidiDebuggerMaxNum(int num)
	: num(num) {}

bool ActionSetMidiDebuggerMaxNum::doAction() {
	juce::String result;

	AudioCore::getInstance()->setMIDIDebuggerMaxNum(this->num);
	result += "MIDI Debugger Max Num: " + juce::String(AudioCore::getInstance()->getMIDIDebuggerMaxNum()) + "\n";
	this->output(result);
	return true;
}
