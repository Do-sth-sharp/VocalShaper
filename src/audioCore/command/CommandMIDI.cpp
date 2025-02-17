#include "CommandUtils.h"
#include "../Utils.h"

AUDIOCORE_FUNC(midiAddNote) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionMIDIAddNote{
		(uint64_t)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(double)luaL_checknumber(L, 3), (double)luaL_checknumber(L, 4),
		(uint8_t)luaL_checkinteger(L, 5), (uint8_t)luaL_checkinteger(L, 6),
		(uint8_t)luaL_checkinteger(L, 7),
		juce::String::fromUTF8(luaL_checkstring(L, 8)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(midiSetNoteTime) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionMIDISetNoteTime{
		(uint64_t)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3),
		(double)luaL_checknumber(L, 4), (double)luaL_checknumber(L, 5) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(midiSetNoteChannel) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionMIDISetNoteChannel{
		(uint64_t)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (uint8_t)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(midiSetNotePitch) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionMIDISetNotePitch{
		(uint64_t)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (uint8_t)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(midiSetNoteVelocity) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionMIDISetNoteVelocity{
		(uint64_t)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (uint8_t)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(midiSetNoteLyrics) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionMIDISetNoteLyrics{
		(uint64_t)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3),
		juce::String::fromUTF8(luaL_checkstring(L, 4)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(midiRemoveNote) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionMIDIRemoveNote{
		(uint64_t)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

void regCommandMIDI(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, midiAddNote);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, midiSetNoteTime);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, midiSetNoteChannel);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, midiSetNotePitch);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, midiSetNoteVelocity);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, midiSetNoteLyrics);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, midiRemoveNote);
}