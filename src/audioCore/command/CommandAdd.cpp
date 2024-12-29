#include "CommandUtils.h"
#include "../Utils.h"

AUDIOCORE_FUNC(addPluginBlackList) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddPluginBlackList{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addPluginSearchPath) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddPluginSearchPath{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addTrack) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddTrack{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addTrackAudioInput) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddTrackAudioInput{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addTrackMIDIInput) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddTrackMIDIInput{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addTrackAudioSend) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddTrackAudioSend{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3),
		{ (quickAPI::SendDst::first_type)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5) },
		(int)luaL_checkinteger(L, 6), (int)luaL_checkinteger(L, 7) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addTrackMIDISend) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddTrackMIDISend{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3),
		{ (quickAPI::SendDst::first_type)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5) } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addEffect) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddEffect{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), juce::String::fromUTF8(luaL_checkstring(L, 4)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addInstr) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddInstr{
		(int)luaL_checkinteger(L, 1), juce::String::fromUTF8(luaL_checkstring(L, 2)),
		(bool)lua_toboolean(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addTrackSideChainBus) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddTrackSideChainBus{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addSequencerBlock) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddSequencerBlock{
		(int)luaL_checkinteger(L, 1), (double)luaL_checknumber(L, 2),
		(double)luaL_checknumber(L, 3), (double)luaL_checknumber(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addTempoTempo) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddTempoTempo{
		(double)luaL_checknumber(L, 1), (double)luaL_checknumber(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addTempoBeat) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionAddTempoBeat{
		(double)luaL_checknumber(L, 1), 
		(int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

void regCommandAdd(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addPluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addPluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addTrackAudioInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addTrackMIDIInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addTrackAudioSend);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addTrackMIDISend);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addEffect);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addInstr);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addTrackSideChainBus);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addSequencerBlock);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addTempoTempo);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addTempoBeat);
}
