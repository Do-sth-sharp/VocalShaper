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

AUDIOCORE_FUNC(setMIDIDebuggerMaxNum) {
	juce::String result;

	audioCore->setMIDIDebuggerMaxNum(luaL_checkinteger(L, 1));
	result += "MIDI Debugger Max Num: " + juce::String(audioCore->getMIDIDebuggerMaxNum()) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setMixerTrackGain) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int trackIndex = luaL_checkinteger(L, 1);
		double value = luaL_checknumber(L, 2);
		
		auto track = graph->getTrackProcessor(trackIndex);
		if (track) {
			track->setGain(static_cast<float>(value));

			result += "Set Mixer Track Gain Value: <" + juce::String(trackIndex) + "> " + juce::String(track->getGain()) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setMixerTrackPan) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int trackIndex = luaL_checkinteger(L, 1);
		double value = luaL_checknumber(L, 2);

		auto track = graph->getTrackProcessor(trackIndex);
		if (track) {
			track->setPan(static_cast<float>(value));

			result += "Set Mixer Track Pan Value: <" + juce::String(trackIndex) + "> " + juce::String(track->getPan()) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setMixerTrackSlider) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int trackIndex = luaL_checkinteger(L, 1);
		double value = luaL_checknumber(L, 2);

		auto track = graph->getTrackProcessor(trackIndex);
		if (track) {
			track->setSlider(static_cast<float>(value));

			result += "Set Mixer Track Slider Value: <" + juce::String(trackIndex) + "> " + juce::String(track->getSlider()) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setEffectWindow) {
	juce::String result;

	int trackIndex = luaL_checkinteger(L, 1);
	int effectIndex = luaL_checkinteger(L, 2);
	auto plugin = AudioCore::getInstance()->getEffect(trackIndex, effectIndex);
	if (plugin) {
		auto editor = plugin->createEditorIfNeeded();
		if (editor) {
			bool visible = lua_toboolean(L, 3);

			if (visible) {
				editor->setName(plugin->getName());
				editor->addToDesktop(
					juce::ComponentPeer::windowAppearsOnTaskbar |
					juce::ComponentPeer::windowHasTitleBar |
					juce::ComponentPeer::windowHasDropShadow);
			}
			editor->setVisible(visible);

			result += "Plugin Window: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "] " + juce::String(visible ? "ON" : "OFF") + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setEffectBypass) {
	juce::String result;

	int trackIndex = luaL_checkinteger(L, 1);
	int effectIndex = luaL_checkinteger(L, 2);
	bool bypass = lua_toboolean(L, 3);

	AudioCore::getInstance()->bypassEffect(trackIndex, effectIndex, bypass);

	result += "Plugin Bypass: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "] " + juce::String(bypass ? "ON" : "OFF") + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setInstrWindow) {
	juce::String result;

	int instrIndex = luaL_checkinteger(L, 1);
	auto plugin = AudioCore::getInstance()->getInstrument(instrIndex);
	if (plugin) {
		auto editor = plugin->createEditorIfNeeded();
		if (editor) {
			bool visible = lua_toboolean(L, 2);

			if (visible) {
				editor->setName(plugin->getName());
				editor->addToDesktop(
					juce::ComponentPeer::windowAppearsOnTaskbar |
					juce::ComponentPeer::windowHasTitleBar |
					juce::ComponentPeer::windowHasDropShadow);
			}
			editor->setVisible(visible);

			result += "Plugin Window: [" + juce::String(instrIndex) + "] " + juce::String(visible ? "ON" : "OFF") + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setInstrBypass) {
	juce::String result;

	int instrIndex = luaL_checkinteger(L, 1);
	bool bypass = lua_toboolean(L, 2);

	AudioCore::getInstance()->bypassInstrument(instrIndex, bypass);

	result += "Plugin Bypass: [" + juce::String(instrIndex) + "] " + juce::String(bypass ? "ON" : "OFF") + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setInstrMIDIChannel) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int instrIndex = luaL_checkinteger(L, 1);
		int channel = luaL_checkinteger(L, 2);

		auto instr = graph->getInstrumentProcessor(instrIndex);
		if (instr) {
			instr->setMIDIChannel(channel);

			result += "Plugin MIDI Channel: [" + juce::String(instrIndex) + "] " + juce::String(instr->getMIDIChannel()) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setInstrParamValue) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int instrIndex = luaL_checkinteger(L, 1);
		int paramIndex = luaL_checkinteger(L, 2);
		float value = luaL_checknumber(L, 3);

		auto instr = graph->getInstrumentProcessor(instrIndex);
		if (instr) {
			instr->setParamValue(paramIndex, value);
			result += "Set Instr Param Value: [" + juce::String(paramIndex) + "] " + instr->getParamName(paramIndex) + " - " + juce::String(instr->getParamValue(paramIndex)) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setEffectParamValue) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int paramIndex = luaL_checkinteger(L, 3);
		float value = luaL_checknumber(L, 4);

		auto track = graph->getTrackProcessor(trackIndex);
		if (track) {
			auto pluginDock = track->getPluginDock();
			if (pluginDock) {
				auto effect = pluginDock->getPluginProcessor(effectIndex);
				if (effect) {
					effect->setParamValue(paramIndex, value);
					result += "Effect Param Value: [" + juce::String(paramIndex) + "] " + effect->getParamName(paramIndex) + " - " + juce::String(effect->getParamValue(paramIndex)) + "\n";
				}
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setInstrParamConnectToCC) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int instrIndex = luaL_checkinteger(L, 1);
		int paramIndex = luaL_checkinteger(L, 2);
		int CCIndex = luaL_checkinteger(L, 3);

		auto instr = graph->getInstrumentProcessor(instrIndex);
		if (instr) {
			instr->connectParamCC(paramIndex, CCIndex);
			result += "Connect Instr Param To MIDI CC: [" + juce::String(paramIndex) + "] " + instr->getParamName(paramIndex) + " - MIDI CC " + juce::String(instr->getParamCCConnection(paramIndex)) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setEffectParamConnectToCC) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int paramIndex = luaL_checkinteger(L, 3);
		int CCIndex = luaL_checkinteger(L, 4);

		auto track = graph->getTrackProcessor(trackIndex);
		if (track) {
			auto pluginDock = track->getPluginDock();
			if (pluginDock) {
				auto effect = pluginDock->getPluginProcessor(effectIndex);
				if (effect) {
					effect->connectParamCC(paramIndex, CCIndex);
					result += "Connect Effect Param To MIDI CC: [" + juce::String(paramIndex) + "] " + effect->getParamName(paramIndex) + " - MIDI CC " + juce::String(effect->getParamCCConnection(paramIndex)) + "\n";
				}
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setInstrParamListenCC) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int instrIndex = luaL_checkinteger(L, 1);
		int paramIndex = luaL_checkinteger(L, 2);

		auto instr = graph->getInstrumentProcessor(instrIndex);
		if (instr) {
			instr->setParamCCListenning(paramIndex);
			result += "Instr Param Listenning MIDI CC: [" + juce::String(paramIndex) + "] " + instr->getParamName(paramIndex) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setEffectParamListenCC) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int paramIndex = luaL_checkinteger(L, 3);

		auto track = graph->getTrackProcessor(trackIndex);
		if (track) {
			auto pluginDock = track->getPluginDock();
			if (pluginDock) {
				auto effect = pluginDock->getPluginProcessor(effectIndex);
				if (effect) {
					effect->setParamCCListenning(paramIndex);
					result += "Effect Param Listenning MIDI CC: [" + juce::String(paramIndex) + "] " + effect->getParamName(paramIndex) + "\n";
				}
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setInstrMIDICCIntercept) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int instrIndex = luaL_checkinteger(L, 1);
		bool intercept = lua_toboolean(L, 2);

		auto instr = graph->getInstrumentProcessor(instrIndex);
		if (instr) {
			instr->setMIDICCIntercept(intercept);
			result += "Set Instr MIDI CC Intercept: [" + juce::String(instrIndex) + "] " + juce::String(instr->getMIDICCIntercept() ? "ON" : "OFF") + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setEffectMIDICCIntercept) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		bool intercept = lua_toboolean(L, 3);

		auto track = graph->getTrackProcessor(trackIndex);
		if (track) {
			auto pluginDock = track->getPluginDock();
			if (pluginDock) {
				auto effect = pluginDock->getPluginProcessor(effectIndex);
				if (effect) {
					effect->setMIDICCIntercept(intercept);
					result += "Set Effect MIDI CC Intercept: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "] " + juce::String(effect->getMIDICCIntercept() ? "ON" : "OFF") + "\n";
				}
			}
		}
	}

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
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setMixerTrackGain);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setMixerTrackPan);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setMixerTrackSlider);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectWindow);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectBypass);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrWindow);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrBypass);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrMIDIChannel);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrParamConnectToCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectParamConnectToCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrParamListenCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectParamListenCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrMIDICCIntercept);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectMIDICCIntercept);
}
