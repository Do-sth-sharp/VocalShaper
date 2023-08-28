#include "CommandUtils.h"

AUDIOCORE_FUNC(exportSource) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		juce::String sourcePath = juce::String::fromUTF8(luaL_checkstring(L, 2));
		if (auto source = manager->getSource(sourceIndex)) {
			if (source->exportAs(juce::File::getCurrentWorkingDirectory().getChildFile(sourcePath))) {
				result += "Export Source Data: " + sourcePath + "\n";
			}
			else {
				result += "Can't Export Source Data: " + sourcePath + "\n";
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(exportSourceAsync) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		juce::String sourcePath = juce::String::fromUTF8(luaL_checkstring(L, 2));

		AudioIOList::getInstance()->exportt(sourceIndex, sourcePath);

		result += "Create Export Source Data Task: " + sourcePath + "\n";
	}

	return CommandFuncResult{ true, result };
}

void regCommandExport(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, exportSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, exportSourceAsync);
}
