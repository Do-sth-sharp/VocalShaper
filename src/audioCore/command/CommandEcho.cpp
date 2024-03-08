#include "CommandUtils.h"

AUDIOCORE_FUNC(echoDeviceAudio) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoDeviceAudio);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoDeviceMIDI) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoDeviceMidi);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoMixerInfo) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoMixerInfo);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoMixerTrack) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoMixerTrack);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoMixerTrackInfo) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoMixerTrackInfo{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoMixerTrackGain) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoMixerTrackGain{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoMixerTrackPan) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoMixerTrackPan{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoMixerTrackSlider) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoMixerTrackSlider{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoInstrParamValue) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoInstrParamValue{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoInstrParamDefaultValue) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoInstrParamDefaultValue{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoEffectParamValue) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoEffectParamValue{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoEffectParamDefaultValue) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoEffectParamDefaultValue{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoInstrParamCC) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoInstrParamCC{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoEffectParamCC) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoEffectParamCC{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoInstrCCParam) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoInstrCCParam{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(echoEffectCCParam) {
	auto action = std::unique_ptr<ActionBase>(new ActionEchoEffectCCParam{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

void regCommandEcho(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoDeviceAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoDeviceMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerInfo);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrackInfo);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrackGain);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrackPan);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrackSlider);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoInstrParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoInstrParamDefaultValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoEffectParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoEffectParamDefaultValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoInstrParamCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoEffectParamCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoInstrCCParam);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoEffectCCParam);
}
