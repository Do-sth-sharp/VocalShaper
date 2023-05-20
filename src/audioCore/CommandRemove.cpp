#include "CommandUtils.h"

AUDIOCORE_FUNC(removePluginBlackList) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin black list while searching plugin." };
	}

	audioCore->removeFromPluginBlackList(luaL_checkstring(L, 1));
	return CommandFuncResult{ true, "Remove from plugin black list." };
}

AUDIOCORE_FUNC(removePluginSearchPath) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin search path while searching plugin." };
	}

	audioCore->removeFromPluginSearchPath(luaL_checkstring(L, 1));
	return CommandFuncResult{ true, "Remove from plugin search path." };
}

AUDIOCORE_FUNC(removeMixerTrack) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int index = luaL_checkinteger(L, 1);
		graph->removeTrack(index);

		result += "Remove Mixer Track: " + juce::String(index) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackSend) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dst = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);
		graph->removeAudioTrk2TrkConnection(src, dst, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackInputFromDevice) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int srcc = luaL_checkinteger(L, 1);
		int dst = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		graph->removeAudioI2TrkConnection(dst, srcc, dstc);

		result += "[Device] " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackOutput) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		graph->removeAudioTrk2OConnection(src, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + "[Device] " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeEffect) {
	juce::String result;

	int trackIndex = luaL_checkinteger(L, 1);
	int effectIndex = luaL_checkinteger(L, 2);
	AudioCore::getInstance()->removeEffect(trackIndex, effectIndex);

	result += "Remove Plugin: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "]" + "\n";

	return CommandFuncResult{ true, result };
}


AUDIOCORE_FUNC(removeEffectAdditionalInput) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int srcc = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);

		auto track = graph->getTrackProcessor(trackIndex);
		if (track) {
			auto pluginDock = track->getPluginDock();
			if (pluginDock) {
				pluginDock->removeAdditionalBusConnection(effectIndex, srcc, dstc);

				result += "Unlink Plugin Channel: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "] " + juce::String(srcc) + " - " + juce::String(dstc) + "\n";
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeInstr) {
	juce::String result;

	int instrIndex = luaL_checkinteger(L, 1);
	AudioCore::getInstance()->removeInstrument(instrIndex);

	result += "Remove Plugin: [" + juce::String(instrIndex) + "]" + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeInstrOutput) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dst = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);
		graph->removeAudioInstr2TrkConnection(src, dst, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeInstrMidiInput) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int src = luaL_checkinteger(L, 1);
		graph->removeMIDII2InstrConnection(src);

		result += juce::String("[MIDI Input]") + " - " + juce::String(src) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

void regCommandRemove(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackSend);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackInputFromDevice);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeEffect);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeEffectAdditionalInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeInstr);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeInstrOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeInstrMidiInput);
}
