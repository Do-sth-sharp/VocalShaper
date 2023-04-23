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

	auto mixer = audioCore->getMixer();
	if (mixer) {
		int index = luaL_checkinteger(L, 1);
		mixer->removeTrack(index);

		result += "Remove Mixer Track: " + juce::String(index) + "\n";
		result += "Total Mixer Track Num: " + juce::String(mixer->getTrackNum()) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackSend) {
	juce::String result;

	auto mixer = audioCore->getMixer();
	if (mixer) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dst = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);
		mixer->removeTrackSend(src, srcc, dst, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackInputFromDevice) {
	juce::String result;

	auto mixer = audioCore->getMixer();
	if (mixer) {
		int srcc = luaL_checkinteger(L, 1);
		int dst = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		mixer->removeTrackAudioInputFromDevice(srcc, dst, dstc);

		result += "[Device] " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackInputFromSequencer) {
	juce::String result;

	auto mixer = audioCore->getMixer();
	if (mixer) {
		int srcc = luaL_checkinteger(L, 1);
		int dst = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		mixer->removeTrackAudioInputFromSequencer(srcc, dst, dstc);

		result += "[Sequencer] " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackOutput) {
	juce::String result;

	auto mixer = audioCore->getMixer();
	if (mixer) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		mixer->removeTrackAudioOutput(src, srcc, dstc);

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
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackInputFromSequencer);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackOutput);
}
