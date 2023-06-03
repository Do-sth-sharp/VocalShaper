#include "CommandUtils.h"

AUDIOCORE_FUNC(loadSource) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);
		juce::String sourcePath = juce::String::fromUTF8(luaL_checkstring(L, 2));
		if (auto source = manager->getSource(sourceIndex)) {
			if (source->loadFrom(juce::File::getCurrentWorkingDirectory().getChildFile(sourcePath))) {
				result += "Load Source Data: " + sourcePath + "\n";
			}
			else {
				result += "Can't Load Source Data: " + sourcePath + "\n";
			}
		}
	}

	return CommandFuncResult{ true, result };
}

void regCommandLoad(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, loadSource);
}
