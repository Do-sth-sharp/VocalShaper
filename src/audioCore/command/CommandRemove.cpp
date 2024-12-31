#include "CommandUtils.h"

AUDIOCORE_FUNC(removePluginBlackList) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemovePluginBlackList{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removePluginSearchPath) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemovePluginSearchPath{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeTrack) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveTrack{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeTrackAudioInput) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveTrackAudioInput{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeTrackMIDIInput) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveTrackMIDIInput{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeTrackAudioSend) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveTrackAudioSend{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3),
		{ (quickAPI::SendDst::first_type)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5) },
		(int)luaL_checkinteger(L, 6), (int)luaL_checkinteger(L, 7) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeTrackMIDISend) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveTrackMIDISend{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3),
		{ (quickAPI::SendDst::first_type)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5) } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeTrackAudioSendAllChannel) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveTrackAudioSendAllChannel{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3),
		{ (quickAPI::SendDst::first_type)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5) } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeTrackAudioSendOnSlot) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveTrackAudioSendOnSlot{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeTrackMIDISendOnSlot) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveTrackMIDISendOnSlot{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeEffect) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveEffect{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeInstr) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveInstr{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeTrackSideChainBus) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveTrackSideChainBus{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeInstrParamCCConnection) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveInstrParamCCConnection{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeEffectParamCCConnection) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveEffectParamCCConnection{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeSequencerBlock) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveSequencerBlock{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(removeLabel) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRemoveLabel{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

void regCommandRemove(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removePluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeTrackAudioInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeTrackMIDIInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeTrackAudioSend);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeTrackMIDISend);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeTrackAudioSendAllChannel);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeTrackAudioSendOnSlot);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeTrackMIDISendOnSlot);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeEffect);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeInstr);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeTrackSideChainBus);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeInstrParamCCConnection);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeEffectParamCCConnection);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerBlock);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeLabel);
}
