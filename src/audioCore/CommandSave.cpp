#include "CommandUtils.h"

AUDIOCORE_FUNC(saveSource) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		juce::String sourcePath = juce::String::fromUTF8(luaL_checkstring(L, 2));
		if (auto source = manager->getSource(sourceIndex)) {
			if (source->saveAs(juce::File::getCurrentWorkingDirectory().getChildFile(sourcePath))) {
				result += "Save Source Data: " + sourcePath + "\n";
			}
			else {
				result += "Can't Save Source Data: " + sourcePath + "\n";
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(saveSourceAsync) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		juce::String sourcePath = juce::String::fromUTF8(luaL_checkstring(L, 2));

		AudioIOList::getInstance()->save(sourceIndex, sourcePath);

		result += "Create Save Source Data Task: " + sourcePath + "\n";
	}

	return CommandFuncResult{ true, result };
}

void regCommandSave(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, saveSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, saveSourceAsync);
}
