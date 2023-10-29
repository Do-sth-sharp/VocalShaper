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
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		manager->removeSource(luaL_checkinteger(L, 1));
		result += "Total Source Num: " + juce::String(manager->getSourceNum()) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeSequencerTrack) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int index = luaL_checkinteger(L, 1);
		graph->removeSource(index);

		result += "Remove Sequencer Track: " + juce::String(index) + "\n";
		result += "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeSequencerTrackMidiOutputToMixer) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int dst = luaL_checkinteger(L, 2);
		graph->removeMIDISrc2TrkConnection(src, dst);

		result += juce::String(src) + " - " + juce::String(dst) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeSequencerTrackMidiOutputToInstr) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int dst = luaL_checkinteger(L, 2);
		graph->removeMIDISrc2InstrConnection(src, dst);

		result += juce::String(src) + " - " + juce::String(dst) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeSequencerTrackOutput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dst = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);
		graph->removeAudioSrc2TrkConnection(src, dst, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeSequencerSourceInstance) {
	juce::String result;

	int trackIndex = luaL_checkinteger(L, 1);
	int seqIndex = luaL_checkinteger(L, 2);

	AudioCore::getInstance()->removeSequencerSourceInstance(trackIndex, seqIndex);

	result += "Remove Sequencer Source Instance [" + juce::String(trackIndex) + ", " + juce::String(seqIndex) + "]\n";
	result += "Total Sequencer Source Instance: " + juce::String(AudioCore::getInstance()->getSequencerSourceInstanceNum(trackIndex)) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeRecorderSourceInstance) {
	juce::String result;

	int seqIndex = luaL_checkinteger(L, 1);

	AudioCore::getInstance()->removeRecorderSourceInstance(seqIndex);

	result += "Remove Recorder Source Instance [" + juce::String(seqIndex) + "]\n";
	result += "Total Recorder Source Instance: " + juce::String(AudioCore::getInstance()->getRecorderSourceInstanceNum()) + "\n";

	return CommandFuncResult{ true, result };
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
