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

AUDIOCORE_FUNC(synthSource) {
	juce::String result;

	int srcIndex = luaL_checkinteger(L, 1);
	if (CloneableSourceManager::getInstance()->synthSource(srcIndex)) {
		result += "Start synth source: [" + juce::String(srcIndex) + "]\n";
	}
	else {
		result += "Can't start synth source: [" + juce::String(srcIndex) + "]\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(cloneSource) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		if (CloneableSourceManager::getInstance()
				->cloneSource(sourceIndex)) {
			result += "Clone Source: [" + juce::String(sourceIndex) + "]\n";
			result += "Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n";
		}
		else {
			result += "Can't Clone Source: [" + juce::String(sourceIndex) + "]\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(saveSource) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		juce::String sourcePath = juce::String::fromUTF8(luaL_checkstring(L, 2));
		if (CloneableSourceManager::getInstance()
			->saveSource(sourceIndex, sourcePath)) {
			result += "Save Source Data: " + sourcePath + "\n";
		}
		else {
			result += "Can't Save Source Data: " + sourcePath + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(saveSourceAsync) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		juce::String sourcePath = juce::String::fromUTF8(luaL_checkstring(L, 2));

		CloneableSourceManager::getInstance()
			->saveSourceAsync(sourceIndex, sourcePath);

		result += "Create Save Source Data Task: " + sourcePath + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(exportSource) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		juce::String sourcePath = juce::String::fromUTF8(luaL_checkstring(L, 2));
		if (CloneableSourceManager::getInstance()
			->exportSource(sourceIndex, sourcePath)) {
			result += "Export Source Data: " + sourcePath + "\n";
		}
		else {
			result += "Can't Export Source Data: " + sourcePath + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(exportSourceAsync) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		juce::String sourcePath = juce::String::fromUTF8(luaL_checkstring(L, 2));

		CloneableSourceManager::getInstance()
			->exportSourceAsync(sourceIndex, sourcePath);

		result += "Create Export Source Data Task: " + sourcePath + "\n";
	}

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
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, synthSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, cloneSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, saveSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, saveSourceAsync);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, exportSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, exportSourceAsync);
}
