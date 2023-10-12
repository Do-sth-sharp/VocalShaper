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

AUDIOCORE_FUNC(renderNow) {
	juce::String result;

	juce::String path = juce::String::fromUTF8(luaL_checkstring(L, 1));
	juce::String name = juce::String::fromUTF8(luaL_checkstring(L, 2));
	juce::String extension = juce::String::fromUTF8(luaL_checkstring(L, 3));
	
	juce::Array<int> tracks;
	lua_pushvalue(L, 4);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		tracks.add(luaL_checkinteger(L, -1));
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	if (AudioCore::getInstance()->renderNow(tracks, path, name, extension)) {
		result += "Start rendering:\n";
		result += "    Path: " + path + "\n";
		result += "    Name: " + name + "\n";
		result += "    Format: " + extension + "\n";
		result += "    Tracks: ";
		for (auto& i : tracks) {
			result += juce::String(i) + " ";
		}
		result += "\n";
	}
	else {
		result += "Can't start to render. Maybe rendering is already started!\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(newProject) {
	juce::String result;

	juce::String path = juce::String::fromUTF8(luaL_checkstring(L, 1));
	if (AudioCore::getInstance()->newProj(path)) {
		result += "Create new project at: " + path + "\n";
	}
	else {
		result += "Can't create new project at: " + path + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(save) {
	juce::String result;

	juce::String name = juce::String::fromUTF8(luaL_checkstring(L, 1));
	if (AudioCore::getInstance()->save(name)) {
		result += "Saved project data to: " + name + "\n";
	}
	else {
		result += "Can't save project data to: " + name + "\n";
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
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, renderNow);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, newProject);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, save);
}
