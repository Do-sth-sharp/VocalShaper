#include "CommandUtils.h"

AUDIOCORE_FUNC(removePluginBlackList) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemovePluginBlackList{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removePluginSearchPath) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemovePluginSearchPath{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeMixerTrack) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveMixerTrack{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeMixerTrackSend) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackSend{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeMixerTrackInputFromDevice) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackInputFromDevice{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeMixerTrackOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackOutput{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeEffect) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveEffect{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeEffectAdditionalInput) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveEffectAdditionalInput{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeInstr) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveInstr{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeInstrOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveInstrOutput{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeInstrMidiInput) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveInstrMidiInput{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeInstrParamCCConnection) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveInstrParamCCConnection{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeEffectParamCCConnection) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveEffectParamCCConnection{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeMixerTrackMidiInput) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackMidiInput{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeMixerTrackMidiOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveMixerTrackMidiOutput{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeSource) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveSource{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeSequencerTrack) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveSequencerTrack{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeSequencerTrackMidiOutputToMixer) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveSequencerTrackMidiOutputToMixer{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeSequencerTrackMidiOutputToInstr) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveSequencerTrackMidiOutputToInstr{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeSequencerTrackOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveSequencerTrackOutput{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeSequencerSourceInstance) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveSequencerSourceInstance{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeRecorderSourceInstance) {
	auto action = std::unique_ptr<ActionBase>(new ActionRemoveRecorderSourceInstance{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

void regCommandRemove(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackSend);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackInputFromDevice);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeEffect);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeEffectAdditionalInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeInstr);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeInstrOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeInstrMidiInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeInstrParamCCConnection);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeEffectParamCCConnection);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackMidiInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeMixerTrackMidiOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerTrackMidiOutputToMixer);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerTrackMidiOutputToInstr);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerTrackOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerSourceInstance);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeRecorderSourceInstance);
}
