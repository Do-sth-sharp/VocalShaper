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

void regCommandRemove(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackSend);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackInputFromDevice);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackOutput);
}