#include "CommandUtils.h"

AUDIOCORE_FUNC(setDeviceAudioType) {
	juce::String result;
	audioCore->setCurrentAudioDeviceType(luaL_checkstring(L, 1));

	result += "Current Audio Device Type: " + getAudioDeviceManager(audioCore)->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + audioCore->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + audioCore->getAudioOutputDeviceName() + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceAudioInput) {
	juce::String result;
	auto err = audioCore->setAudioInputDevice(luaL_checkstring(L, 1));
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Device Type: " + getAudioDeviceManager(audioCore)->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + audioCore->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + audioCore->getAudioOutputDeviceName() + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceAudioOutput) {
	juce::String result;
	auto err = audioCore->setAudioOutputDevice(luaL_checkstring(L, 1));
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Device Type: " + getAudioDeviceManager(audioCore)->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + audioCore->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + audioCore->getAudioOutputDeviceName() + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceAudioSampleRate) {
	juce::String result;
	auto err = audioCore->setAudioSampleRate(luaL_checknumber(L, 1));
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Sample Rate: " + juce::String(audioCore->getAudioSampleRate()) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceAudioBufferSize) {
	juce::String result;
	auto err = audioCore->setAudioBufferSize(luaL_checkinteger(L, 1));
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Buffer Size: " + juce::String(audioCore->getAudioBufferSize()) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceMIDIInput) {
	juce::String result;

	juce::String device = luaL_checkstring(L, 1);
	audioCore->setMIDIInputDeviceEnabled(device, lua_toboolean(L, 2));
	result += "MIDI Input Device: " + device + " - " + (audioCore->getMIDIInputDeviceEnabled(device) ? "ON" : "OFF") + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceMIDIOutput) {
	juce::String result;

	juce::String device = luaL_checkstring(L, 1);
	audioCore->setMIDIOutputDevice(device);
	result += "MIDI Output Device: " + device + "\n";

	return CommandFuncResult{ true, result };
}

void regCommandSet(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioType);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioSampleRate);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioBufferSize);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceMIDIInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceMIDIOutput);
}
