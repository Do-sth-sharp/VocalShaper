#include "CommandUtils.h"

AUDIOCORE_FUNC(clearPlugin) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionClearPlugin);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(searchPlugin) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSearchPlugin);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(play) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionPlay);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(pause) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionPause);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(stop) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionStop);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(rewind) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRewind);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(startRecord) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionStartRecord);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(stopRecord) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionStopRecord);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(renderNow) {
	juce::String result;

	juce::String path = juce::String::fromUTF8(luaL_checkstring(L, 1));
	juce::String name = juce::String::fromUTF8(luaL_checkstring(L, 2));
	juce::String extension = juce::String::fromUTF8(luaL_checkstring(L, 3));
	
	juce::Array<quickAPI::TrackIndex> tracks;
	lua_pushvalue(L, 4);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		lua_getfield(L, -1, "type");
		int type = luaL_checkinteger(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "index");
		int index = luaL_checkinteger(L, -1);
		lua_pop(L, 1);

		tracks.add({ (quickAPI::TrackType)type, index });
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	juce::StringPairArray metaData;
	lua_pushvalue(L, 5);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		metaData.set(luaL_checkstring(L, -2), luaL_checkstring(L, -1));
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	int bitDepth = luaL_checkinteger(L, 6);
	int quality = luaL_checkinteger(L, 7);

	auto action = std::unique_ptr<ActionUndoableBase>(new ActionRenderNow{
		path, name, extension, tracks, metaData, bitDepth, quality });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(newProject) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionNewProject{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(save) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSave{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(load) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionLoad{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(initAudio) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionInitAudioSource{
		(int)luaL_checkinteger(L, 1), luaL_checkstring(L, 2),
		(double)luaL_checknumber(L, 3), (int)luaL_checkinteger(L, 4), (double)luaL_checknumber(L, 5) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(initMIDI) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionInitMidiSource{
		(int)luaL_checkinteger(L, 1), luaL_checkstring(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(loadAudio) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionLoadAudioSource{
		(int)luaL_checkinteger(L, 1), 
		juce::String::fromUTF8(luaL_checkstring(L, 2)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(loadMIDI) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionLoadMidiSource{
		(int)luaL_checkinteger(L, 1),
		juce::String::fromUTF8(luaL_checkstring(L, 2)),
		(bool)lua_toboolean(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(saveAudio) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSaveAudioSource{
		(int)luaL_checkinteger(L, 1),
		juce::String::fromUTF8(luaL_checkstring(L, 2)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(saveMIDI) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSaveMidiSource{
		(int)luaL_checkinteger(L, 1),
		juce::String::fromUTF8(luaL_checkstring(L, 2)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(splitSequencerBlock) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSplitSequencerBlock{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(double)luaL_checknumber(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

void regCommandOther(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, clearPlugin);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, searchPlugin);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, play);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, pause);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, stop);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, rewind);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, startRecord);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, stopRecord);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, renderNow);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, newProject);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, save);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, load);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, initAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, initMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, loadAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, loadMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, saveAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, saveMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, splitSequencerBlock);
}
