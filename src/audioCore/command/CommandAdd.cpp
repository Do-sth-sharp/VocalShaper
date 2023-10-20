#include "CommandUtils.h"
#include "../Utils.h"

AUDIOCORE_FUNC(addPluginBlackList) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddPluginBlackList{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addPluginSearchPath) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddPluginSearchPath{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addMixerTrack) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddMixerTrack{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addMixerTrackSend) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddMixerTrackSend{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addMixerTrackInputFromDevice) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddMixerTrackInputFromDevice{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addMixerTrackOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddMixerTrackOutput{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addEffect) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddEffect{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		juce::String::fromUTF8(luaL_checkstring(L, 3)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addEffectAdditionalInput) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddEffectAdditionalInput{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addInstr) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddInstr{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		juce::String::fromUTF8(luaL_checkstring(L, 3)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addInstrOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddInstrOutput{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addInstrMidiInput) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddInstrMidiInput{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addMixerTrackMidiInput) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddMixerTrackMidiInput{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addMixerTrackMidiOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddMixerTrackMidiOutput{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addAudioSource) {
	if (lua_isstring(L, 1) && !lua_isnumber(L, 1)) {
		auto action = std::unique_ptr<ActionBase>(new ActionAddAudioSourceThenLoad{
		juce::String::fromUTF8(lua_tostring(L, 1)), (bool)lua_toboolean(L, 2) });
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}
	else {
		auto action = std::unique_ptr<ActionBase>(new ActionAddAudioSourceThenInit{
		luaL_checknumber(L, 1), (int)luaL_checkinteger(L, 2),
		luaL_checknumber(L, 3) });
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}

	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addMIDISource) {
	if (lua_isstring(L, 1) && !lua_isnumber(L, 1)) {
		auto action = std::unique_ptr<ActionBase>(new ActionAddMidiSourceThenLoad{
		juce::String::fromUTF8(lua_tostring(L, 1)), (bool)lua_toboolean(L, 2) });
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}
	else {
		auto action = std::unique_ptr<ActionBase>(new ActionAddMidiSourceThenInit);
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}

	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addSynthSource) {
	if (lua_isstring(L, 1) && !lua_isnumber(L, 1)) {
		auto action = std::unique_ptr<ActionBase>(new ActionAddSynthSourceThenLoad{
		juce::String::fromUTF8(lua_tostring(L, 1)), (bool)lua_toboolean(L, 2) });
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}
	else {
		auto action = std::unique_ptr<ActionBase>(new ActionAddSynthSourceThenInit);
		ActionDispatcher::getInstance()->dispatch(std::move(action));
	}

	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addSequencerTrack) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddSequencerTrack{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addSequencerTrackMidiOutputToMixer) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddSequencerTrackMidiOutputToMixer{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addSequencerTrackMidiOutputToInstr) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddSequencerTrackMidiOutputToInstr{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addSequencerTrackOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddSequencerTrackOutput{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addSequencerSourceInstance) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddSequencerSourceInstance{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		luaL_checknumber(L, 3), luaL_checknumber(L, 4),
		luaL_checknumber(L, 5) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(addRecorderSourceInstance) {
	auto action = std::unique_ptr<ActionBase>(new ActionAddRecorderSourceInstance{
		(int)luaL_checkinteger(L, 1), luaL_checknumber(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

void regCommandAdd(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addPluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addPluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrackSend);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrackInputFromDevice);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrackOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addEffect);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addEffectAdditionalInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addInstr);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addInstrOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addInstrMidiInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrackMidiInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrackMidiOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addAudioSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMIDISource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addSynthSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addSequencerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addSequencerTrackMidiOutputToMixer);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addSequencerTrackMidiOutputToInstr);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addSequencerTrackOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addSequencerSourceInstance);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addRecorderSourceInstance);
}
