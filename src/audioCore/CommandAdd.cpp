#include "CommandUtils.h"

AUDIOCORE_FUNC(addPluginBlackList) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin black list while searching plugin." };
	}

	audioCore->addToPluginBlackList(luaL_checkstring(L, 1));
	return CommandFuncResult{ true, "Add to plugin black list." };
}

AUDIOCORE_FUNC(addPluginSearchPath) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin search path while searching plugin." };
	}

	audioCore->addToPluginSearchPath(luaL_checkstring(L, 1));
	return CommandFuncResult{ true, "Add to plugin search path." };
}

void regCommandAdd(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addPluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addPluginSearchPath);
}
