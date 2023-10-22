#include "CommandUtils.h"

AUDIOCORE_FUNC(listDeviceAudio) {
	auto action = std::unique_ptr<ActionBase>(new ActionListDeviceAudio);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(listDeviceMIDI) {
	auto action = std::unique_ptr<ActionBase>(new ActionListDeviceMidi);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(listPluginBlackList) {
	auto action = std::unique_ptr<ActionBase>(new ActionListPluginBlackList);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(listPluginSearchPath) {
	auto action = std::unique_ptr<ActionBase>(new ActionListPluginSearchPath);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(listPlugin) {
	auto action = std::unique_ptr<ActionBase>(new ActionListPlugin);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(listInstrParam) {
	auto action = std::unique_ptr<ActionBase>(new ActionListInstrParam{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(listEffectParam) {
	auto action = std::unique_ptr<ActionBase>(new ActionListEffectParam{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

void regCommandList(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listDeviceAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listDeviceMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listPluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listPluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listPlugin);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listInstrParam);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listEffectParam);
}
