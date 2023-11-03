#include "CommandUtils.h"

#include "../misc/Device.h"
#include "../Utils.h"

AUDIOCORE_FUNC(setDeviceAudioType) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetDeviceAudioType{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceAudioInput) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetDeviceAudioInput{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceAudioOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetDeviceAudioOutput{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceAudioSampleRate) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetDeviceAudioSampleRate{
		luaL_checknumber(L, 1)});
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceAudioBufferSize) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetDeviceAudioBufferSize{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceMIDIInput) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetDeviceMidiInput{
		juce::String::fromUTF8(luaL_checkstring(L, 1)), (bool)lua_toboolean(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setDeviceMIDIOutput) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetDeviceMidiOutput{
		juce::String::fromUTF8(luaL_checkstring(L, 1)) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setMIDIDebuggerMaxNum) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetMidiDebuggerMaxNum{
		(int)luaL_checkinteger(L, 1) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setMixerTrackGain) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetMixerTrackGain{
		(int)luaL_checkinteger(L, 1), (float)luaL_checknumber(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setMixerTrackPan) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetMixerTrackPan{
		(int)luaL_checkinteger(L, 1), (float)luaL_checknumber(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setMixerTrackSlider) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetMixerTrackSlider{
		(int)luaL_checkinteger(L, 1), (float)luaL_checknumber(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectWindow) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetEffectWindow{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(bool)lua_toboolean(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectBypass) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetEffectBypass{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(bool)lua_toboolean(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrWindow) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetInstrWindow{
		(int)luaL_checkinteger(L, 1), (bool)lua_toboolean(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrBypass) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetInstrBypass{
		(int)luaL_checkinteger(L, 1), (bool)lua_toboolean(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrMIDIChannel) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetInstrMidiChannel{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectMIDIChannel) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetEffectMidiChannel{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrParamValue) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetInstrParamValue{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(float)luaL_checknumber(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectParamValue) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetEffectParamValue{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (float)luaL_checknumber(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrParamConnectToCC) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetInstrParamConnectToCC{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectParamConnectToCC) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetEffectParamConnectToCC{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setInstrMIDICCIntercept) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetInstrMidiCCIntercept{
		(int)luaL_checkinteger(L, 1), (bool)lua_toboolean(L, 2) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setEffectMIDICCIntercept) {
	auto action = std::unique_ptr<ActionBase>(new ActionSetEffectMidiCCIntercept{
		(int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
		(bool)lua_toboolean(L, 3) });
	ActionDispatcher::getInstance()->dispatch(std::move(action));
	return CommandFuncResult{ true, "" };
}

AUDIOCORE_FUNC(setSequencerTrackBypass) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);
		bool bypass = lua_toboolean(L, 2);

		graph->setSourceBypass(trackIndex, bypass);

		result += "Sequencer Track Bypass: [" + juce::String(trackIndex) + "] " + juce::String(graph->getSourceBypass(trackIndex) ? "ON" : "OFF") + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setPlayPosition) {
	juce::String result;

	audioCore->setPositon(luaL_checknumber(L, 1));
	auto pos = audioCore->getPosition();

	result += "Set play position at " + juce::String(pos->getTimeInSeconds().orFallback(0)) + " seconds\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setReturnToStart) {
	juce::String result;

	audioCore->setReturnToPlayStartPosition(lua_toboolean(L, 1));

	result += "Set return to start position on play stop: " + juce::String(AudioCore::getInstance()->getReturnToPlayStartPosition() ? "ON" : "OFF") + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setSourceSynthesizer) {
	juce::String result;

	int srcIndex = luaL_checkinteger(L, 1);
	juce::String pid = juce::String::fromUTF8(luaL_checkstring(L, 2));
	if (CloneableSourceManager::getInstance()->setSourceSynthesizer(srcIndex, pid)) {
		result += "Set synthesizer: [" + juce::String(srcIndex) + "] " + pid + "\n";
	}
	else {
		result += "Can't set synthesizer: [" + juce::String(srcIndex) + "] " + pid + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setAudioSaveBitsPerSample) {
	juce::String result;

	juce::String format = juce::String::fromUTF8(luaL_checkstring(L, 1));
	int bitsPerSample = luaL_checkinteger(L, 2);

	utils::AudioSaveConfig::getInstance()->setBitsPerSample(format, bitsPerSample);
	
	result += "Set audio save bits per sample: [" + format + "] " 
		+ juce::String(utils::AudioSaveConfig::getInstance()->getBitsPerSample(format)) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setAudioSaveMetaData) {
	juce::String result;

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

	utils::AudioSaveConfig::getInstance()->setMetaData(format, metaData);

	metaData = utils::AudioSaveConfig::getInstance()->getMetaData(format);
	auto& metaKeys = metaData.getAllKeys();
	result += "Set audio save meta data: [" + format + "]\n";
	for (auto& i : metaKeys) {
		result += "    " + i + " : " + metaData.getValue(i, "") + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setAudioSaveQualityOptionIndex) {
	juce::String result;

	juce::String format = juce::String::fromUTF8(luaL_checkstring(L, 1));
	int qualityOptionIndex = luaL_checkinteger(L, 2);

	utils::AudioSaveConfig::getInstance()
		->setQualityOptionIndex(format, qualityOptionIndex);

	result += "Set audio save quality option index: [" + format + "] "
		+ juce::String(utils::AudioSaveConfig::getInstance()->getQualityOptionIndex(format)) + "\n";

	return CommandFuncResult{ true, result };
}

void regCommandSet(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioType);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioSampleRate);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceAudioBufferSize);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceMIDIInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setDeviceMIDIOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setMixerTrackGain);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setMixerTrackPan);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setMixerTrackSlider);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectWindow);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectBypass);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrWindow);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrBypass);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrMIDIChannel);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectMIDIChannel);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrParamConnectToCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectParamConnectToCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setInstrMIDICCIntercept);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setEffectMIDICCIntercept);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setSequencerTrackBypass);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setPlayPosition);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setReturnToStart);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setSourceSynthesizer);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setAudioSaveBitsPerSample);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setAudioSaveMetaData);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, setAudioSaveQualityOptionIndex);
}
