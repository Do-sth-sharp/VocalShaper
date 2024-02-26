#include "CommandUtils.h"

AUDIOCORE_FUNC(clearPlugin) {
	auto action = std::unique_ptr<ActionBase>(new ActionClearPlugin);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(searchPlugin) {
	auto action = std::unique_ptr<ActionBase>(new ActionSearchPlugin);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(play) {
	auto action = std::unique_ptr<ActionBase>(new ActionPlay);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(pause) {
	auto action = std::unique_ptr<ActionBase>(new ActionPause);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(stop) {
	auto action = std::unique_ptr<ActionBase>(new ActionStop);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(rewind) {
	auto action = std::unique_ptr<ActionBase>(new ActionRewind);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(startRecord) {
	auto action = std::unique_ptr<ActionBase>(new ActionStartRecord);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(stopRecord) {
	auto action = std::unique_ptr<ActionBase>(new ActionStopRecord);
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(renderNow) {
	juce::String result;

	juce::String path = juce::String::fromUTF8(luaL_checkstring(L, 1));
	juce::String name = juce::String::fromUTF8(luaL_checkstring(L, 2));
	juce::String extension = juce::String::fromUTF8(luaL_checkstring(L, 3));
	
	juce::Array<int> tracks;
	lua_pushvalue(L, 4);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		tracks.add(luaL_checkinteger(L, -1));
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	auto action = std::unique_ptr<ActionBase>(new ActionRenderNow{
		path, name, extension, tracks });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(newProject) {
	auto action = std::unique_ptr<ActionBase>(new ActionNewProject{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(save) {
	auto action = std::unique_ptr<ActionBase>(new ActionSave{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(load) {
	auto action = std::unique_ptr<ActionBase>(new ActionLoad{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
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
}
