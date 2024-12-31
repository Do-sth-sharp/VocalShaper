#include "CommandUtils.h"

#include "../misc/Device.h"
#include "../Utils.h"

AUDIOCORE_FUNC(setDeviceAudioType) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetDeviceAudioType{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceAudioInput) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetDeviceAudioInput{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceAudioOutput) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetDeviceAudioOutput{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceAudioSampleRate) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetDeviceAudioSampleRate{
		luaL_checknumber(L, 1)});
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceAudioBufferSize) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetDeviceAudioBufferSize{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceMIDIInput) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetDeviceMidiInput{
		juce::String::fromUTF8(luaL_checkstring(L, 1)), (bool)lua_toboolean(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceMIDIOutput) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetDeviceMidiOutput{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setMIDIDebuggerMaxNum) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetMidiDebuggerMaxNum{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setTrackGain) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetTrackGain{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(float)luaL_checknumber(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setTrackPan) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetTrackPan{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(float)luaL_checknumber(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setTrackFader) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetTrackFader{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(float)luaL_checknumber(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectBypass) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetEffectBypass{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), (bool)lua_toboolean(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrBypass) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetInstrBypass{
		(int)luaL_checkinteger(L, 1), (bool)lua_toboolean(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrMIDIChannel) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetInstrMidiChannel{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectMIDIChannel) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetEffectMidiChannel{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrParamValue) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetInstrParamValue{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(float)luaL_checknumber(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectParamValue) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetEffectParamValue{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4),
		(float)luaL_checknumber(L, 5) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrParamConnectToCC) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetInstrParamConnectToCC{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectParamConnectToCC) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetEffectParamConnectToCC{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4),
		(int)luaL_checkinteger(L, 5) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrMIDICCIntercept) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetInstrMidiCCIntercept{
		(int)luaL_checkinteger(L, 1), (bool)lua_toboolean(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectMIDICCIntercept) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetEffectMidiCCIntercept{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), (bool)lua_toboolean(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setTrackName) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetTrackName{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		juce::String::fromUTF8(luaL_checkstring(L, 3)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setTrackColor) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetTrackColor{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		juce::Colour::fromString(luaL_checkstring(L, 3)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectIndex) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetEffectIndex{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrOffline) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetInstrOffline{
		(int)luaL_checkinteger(L, 1), (bool)lua_toboolean(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setLabelTime) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetLabelTime{
		(int)luaL_checkinteger(L, 1), (double)luaL_checknumber(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setLabelTempo) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetLabelTempo{
		(int)luaL_checkinteger(L, 1), (double)luaL_checknumber(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setLabelBeat) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetLabelBeat{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setTrackMute) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetTrackMute{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(bool)lua_toboolean(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setTrackSolo) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetTrackSolo{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(bool)lua_toboolean(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setTrackRecording) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetTrackRecording{
		(int)luaL_checkinteger(L, 1), (quickAPI::RecordState)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setTrackInputMonitoring) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetTrackInputMonitoring{
		(int)luaL_checkinteger(L, 1), (bool)lua_toboolean(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffect) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetEffect{
		{ (quickAPI::TrackType)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) },
		(int)luaL_checkinteger(L, 3), juce::String::fromUTF8(luaL_checkstring(L, 4)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setCurrentMIDITrack) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetCurrentMIDITrack{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setSequencerBlockTime) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetSequencerBlockTime{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		{ (double)luaL_checknumber(L, 3), (double)luaL_checknumber(L, 4),
		(double)luaL_checknumber(L, 5) } });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setPlayPosition) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetPlayPosition{
		luaL_checknumber(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setReturnToStart) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetReturnToStart{
		(bool)lua_toboolean(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setAudioSaveBitsPerSample) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetAudioSaveBitsPerSample{
		juce::String::fromUTF8(luaL_checkstring(L, 1)), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setAudioSaveMetaData) {
	juce::String format = juce::String::fromUTF8(luaL_checkstring(L, 1));
	juce::StringPairArray metaData;

	lua_pushvalue(L, 2);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		juce::String key = juce::String::fromUTF8(luaL_checkstring(L, -2));
		juce::String val = juce::String::fromUTF8(luaL_checkstring(L, -1));
		metaData.set(key, val);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetAudioSaveMetaData{
		format, metaData });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setAudioSaveQualityOptionIndex) {
	auto action = std::unique_ptr<ActionUndoableBase>(new ActionSetAudioSaveQualityOptionIndex{
		juce::String::fromUTF8(luaL_checkstring(L, 1)), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

void regCommandSet(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioType);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioSampleRate);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioBufferSize);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceMIDIInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceMIDIOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setMIDIDebuggerMaxNum);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setTrackGain);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setTrackPan);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setTrackFader);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectBypass);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrBypass);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrMIDIChannel);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectMIDIChannel);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrParamConnectToCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectParamConnectToCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrMIDICCIntercept);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectMIDICCIntercept);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setTrackName);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setTrackColor);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectIndex);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrOffline);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setLabelTime);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setLabelTempo);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setLabelBeat);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setTrackMute);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setTrackSolo);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setTrackRecording);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setTrackInputMonitoring);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffect);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setCurrentMIDITrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setPlayPosition);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setReturnToStart);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setAudioSaveBitsPerSample);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setAudioSaveMetaData);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setAudioSaveQualityOptionIndex);
}
