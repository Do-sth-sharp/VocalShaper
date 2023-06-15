#include "CommandUtils.h"

AUDIOCORE_FUNC(removePluginBlackList) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin black list while searching plugin." };
	}

	audioCore->removeFromPluginBlackList(juce::String::fromUTF8(luaL_checkstring(L, 1)));
	return CommandFuncResult{ true, "Remove from plugin black list." };
}

AUDIOCORE_FUNC(removePluginSearchPath) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin search path while searching plugin." };
	}

	audioCore->removeFromPluginSearchPath(juce::String::fromUTF8(luaL_checkstring(L, 1)));
	return CommandFuncResult{ true, "Remove from plugin search path." };
}

AUDIOCORE_FUNC(removeMixerTrack) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int index = luaL_checkinteger(L, 1);
		graph->removeTrack(index);

		result += "Remove Mixer Track: " + juce::String(index) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackSend) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dst = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);
		graph->removeAudioTrk2TrkConnection(src, dst, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackInputFromDevice) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int srcc = luaL_checkinteger(L, 1);
		int dst = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		graph->removeAudioI2TrkConnection(dst, srcc, dstc);

		result += "[Device] " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackOutput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		graph->removeAudioTrk2OConnection(src, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + "[Device] " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeEffect) {
	juce::String result;

	int trackIndex = luaL_checkinteger(L, 1);
	int effectIndex = luaL_checkinteger(L, 2);
	AudioCore::getInstance()->removeEffect(trackIndex, effectIndex);

	result += "Remove Plugin: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "]" + "\n";

	return CommandFuncResult{ true, result };
}


AUDIOCORE_FUNC(removeEffectAdditionalInput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int srcc = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->removeAdditionalBusConnection(effectIndex, srcc, dstc);

				result += "Unlink Plugin Channel: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "] " + juce::String(srcc) + " - " + juce::String(dstc) + "\n";
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeInstr) {
	juce::String result;

	int instrIndex = luaL_checkinteger(L, 1);
	AudioCore::getInstance()->removeInstrument(instrIndex);

	result += "Remove Plugin: [" + juce::String(instrIndex) + "]" + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeInstrOutput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dst = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);
		graph->removeAudioInstr2TrkConnection(src, dst, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeInstrMidiInput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int dst = luaL_checkinteger(L, 1);
		graph->removeMIDII2InstrConnection(dst);

		result += juce::String("[MIDI Input]") + " - " + juce::String(dst) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeInstrParamCCConnection) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int instrIndex = luaL_checkinteger(L, 1);
		int CCIndex = luaL_checkinteger(L, 2);

		if (auto instr = graph->getInstrumentProcessor(instrIndex)) {
			instr->removeCCParamConnection(CCIndex);
			result += "Remove Instr Param MIDI CC Connection: " "MIDI CC " + juce::String(CCIndex) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeEffectParamCCConnection) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int CCIndex = luaL_checkinteger(L, 3);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(effectIndex)) {
					effect->removeCCParamConnection(CCIndex);
					result += "Remove Effect Param MIDI CC Connection: " "MIDI CC " + juce::String(CCIndex) + "\n";
				}
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(removeMixerTrackMidiInput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int dst = luaL_checkinteger(L, 1);
		graph->removeMIDII2TrkConnection(dst);

		result += juce::String("[MIDI Input]") + " - " + juce::String(dst) + " (Removed)\n";
	}

	return CommandFuncResult{ true, result };
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
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSource);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerTrackMidiOutputToMixer);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerTrackMidiOutputToInstr);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerTrackOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, removeSequencerSourceInstance);
}
