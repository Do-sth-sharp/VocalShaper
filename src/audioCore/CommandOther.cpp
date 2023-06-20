#include "CommandUtils.h"

AUDIOCORE_FUNC(clearPlugin) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't clear plugin list while searching plugin." };
	}

	audioCore->clearPluginTemporary();
	return CommandFuncResult{ true, "Clear plugin list." };
}

AUDIOCORE_FUNC(searchPlugin) {
	audioCore->clearPluginList();
	audioCore->getPluginList();

	return CommandFuncResult{ true, "Searching Audio Plugin..." };
}

AUDIOCORE_FUNC(play) {
	juce::String result;

	auto pos = audioCore->getPosition();
	audioCore->play();

	result += "Start play at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(pause) {
	juce::String result;

	audioCore->pause();
	auto pos = audioCore->getPosition();

	result += "Pause play at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(stop) {
	juce::String result;

	auto pos = audioCore->getPosition();
	audioCore->stop();

	result += "Stop play at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(rewind) {
	juce::String result;

	audioCore->rewind();

	result += "Rewind play\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(startRecord) {
	juce::String result;

	auto pos = audioCore->getPosition();
	audioCore->record(true);

	result += "Start record at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(stopRecord) {
	juce::String result;

	auto pos = audioCore->getPosition();
	audioCore->record(false);

	result += "Stop record at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n";

	return CommandFuncResult{ true, result };
}

void regCommandOther(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, clearPlugin);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, searchPlugin);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, play);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, pause);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, stop);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, rewind);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, startRecord);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, stopRecord);
}
