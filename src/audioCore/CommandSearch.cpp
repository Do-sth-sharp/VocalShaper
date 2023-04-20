#include "CommandUtils.h"

AUDIOCORE_FUNC(searchPlugin) {
	audioCore->clearPluginList();
	audioCore->getPluginList();

	return CommandFuncResult{ true, "Searching Audio Plugin..." };
}

void regCommandSearch(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, searchPlugin);
}
