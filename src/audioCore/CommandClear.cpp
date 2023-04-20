#include "CommandUtils.h"

AUDIOCORE_FUNC(clearPlugin) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't clear plugin list while searching plugin." };
	}

	audioCore->clearPluginTemporary();
	return CommandFuncResult{ true, "Clear plugin list." };
}

void regCommandClear(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, clearPlugin);
}
