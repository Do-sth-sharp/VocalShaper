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

void regCommandRemove(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginSearchPath);
}
