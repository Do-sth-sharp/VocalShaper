#include "CommandUtils.h"

static juce::AudioChannelSet getChannelSet(int type) {
	juce::AudioChannelSet trackType;

	switch (type) {
	case 0:
		trackType = juce::AudioChannelSet::disabled();
		break;
	case 10:
		trackType = juce::AudioChannelSet::mono();
		break;
	case 20:
		trackType = juce::AudioChannelSet::stereo();
		break;
	case 30:
		trackType = juce::AudioChannelSet::createLCR();
		break;
	case 31:
		trackType = juce::AudioChannelSet::createLRS();
		break;
	case 40:
		trackType = juce::AudioChannelSet::createLCRS();
		break;
	case 50:
		trackType = juce::AudioChannelSet::create5point0();
		break;
	case 51:
		trackType = juce::AudioChannelSet::create5point1();
		break;
	case 502:
		trackType = juce::AudioChannelSet::create5point0point2();
		break;
	case 512:
		trackType = juce::AudioChannelSet::create5point1point2();
		break;
	case 504:
		trackType = juce::AudioChannelSet::create5point0point4();
		break;
	case 514:
		trackType = juce::AudioChannelSet::create5point1point4();
		break;
	case 60:
		trackType = juce::AudioChannelSet::create6point0();
		break;
	case 61:
		trackType = juce::AudioChannelSet::create6point1();
		break;
	case 600:
		trackType = juce::AudioChannelSet::create6point0Music();
		break;
	case 610:
		trackType = juce::AudioChannelSet::create6point1Music();
		break;
	case 70:
		trackType = juce::AudioChannelSet::create7point0();
		break;
	case 700:
		trackType = juce::AudioChannelSet::create7point0SDDS();
		break;
	case 71:
		trackType = juce::AudioChannelSet::create7point1();
		break;
	case 710:
		trackType = juce::AudioChannelSet::create7point1SDDS();
		break;
	case 702:
		trackType = juce::AudioChannelSet::create7point0point2();
		break;
	case 712:
		trackType = juce::AudioChannelSet::create7point1point2();
		break;
	case 704:
		trackType = juce::AudioChannelSet::create7point0point4();
		break;
	case 714:
		trackType = juce::AudioChannelSet::create7point1point4();
		break;
	case 706:
		trackType = juce::AudioChannelSet::create7point0point6();
		break;
	case 716:
		trackType = juce::AudioChannelSet::create7point1point6();
		break;
	case 904:
		trackType = juce::AudioChannelSet::create9point0point4();
		break;
	case 914:
		trackType = juce::AudioChannelSet::create9point1point4();
		break;
	case 906:
		trackType = juce::AudioChannelSet::create9point0point6();
		break;
	case 916:
		trackType = juce::AudioChannelSet::create9point1point6();
		break;
	case 4000:
		trackType = juce::AudioChannelSet::quadraphonic();
		break;
	case 5000:
		trackType = juce::AudioChannelSet::pentagonal();
		break;
	case 6000:
		trackType = juce::AudioChannelSet::hexagonal();
		break;
	case 8000:
		trackType = juce::AudioChannelSet::octagonal();
		break;
	case 100:
	case 101:
	case 102:
	case 103:
	case 104:
	case 105:
	case 106:
	case 107:
		trackType = juce::AudioChannelSet::ambisonic(type - 100);
		break;
	default:
		trackType = juce::AudioChannelSet::stereo();
		break;
	}

	return trackType;
}

AUDIOCORE_FUNC(addPluginBlackList) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin black list while searching plugin." };
	}

	audioCore->addToPluginBlackList(juce::String::fromUTF8(luaL_checkstring(L, 1)));
	return CommandFuncResult{ true, "Add to plugin black list." };
}

AUDIOCORE_FUNC(addPluginSearchPath) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin search path while searching plugin." };
	}

	audioCore->addToPluginSearchPath(juce::String::fromUTF8(luaL_checkstring(L, 1)));
	return CommandFuncResult{ true, "Add to plugin search path." };
}

AUDIOCORE_FUNC(addMixerTrack) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackTypeArg = luaL_checkinteger(L, 2);
		juce::AudioChannelSet trackType = getChannelSet(trackTypeArg);

		graph->insertTrack(luaL_checkinteger(L, 1), trackType);

		result += "Add Mixer Track: [" + juce::String(trackTypeArg) + "]" + trackType.getDescription() + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addMixerTrackSend) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dst = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);
		graph->setAudioTrk2TrkConnection(src, dst, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addMixerTrackInputFromDevice) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int srcc = luaL_checkinteger(L, 1);
		int dst = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		graph->setAudioI2TrkConnection(dst, srcc, dstc);

		result += "[Device] " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addMixerTrackOutput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		graph->setAudioTrk2OConnection(src, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + "[Device] " + juce::String(dstc) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addEffect) {
	juce::String result;

	int trackIndex = luaL_checkinteger(L, 1);
	int effectIndex = luaL_checkinteger(L, 2);
	juce::String pid = juce::String::fromUTF8(luaL_checkstring(L, 3));
	if (AudioCore::getInstance()->addEffect(pid, trackIndex, effectIndex)) {
		result += "Insert Plugin: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "] " + pid + "\n";
	}
	else {
		result += "Insert Plugin: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "] " + pid + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addEffectAdditionalInput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int srcc = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->addAdditionalBusConnection(effectIndex, srcc, dstc);

				result += "Link Plugin Channel: [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "] " + juce::String(srcc) + " - " + juce::String(dstc) + "\n";
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addInstr) {
	juce::String result;

	int instrIndex = luaL_checkinteger(L, 1);
	juce::String pid = juce::String::fromUTF8(luaL_checkstring(L, 2));
	if (AudioCore::getInstance()->addInstrument(pid, instrIndex)) {
		result += "Insert Plugin: [" + juce::String(instrIndex) + "] " + pid + "\n";
	}
	else {
		result += "Can't Insert Plugin: [" + juce::String(instrIndex) + "] " + pid + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addInstrOutput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dst = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);
		graph->setAudioInstr2TrkConnection(src, dst, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addInstrMidiInput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int dst = luaL_checkinteger(L, 1);
		graph->setMIDII2InstrConnection(dst);

		result += juce::String("[MIDI Input]") + " - " + juce::String(dst) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addMixerTrackMidiInput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int dst = luaL_checkinteger(L, 1);
		graph->setMIDII2TrkConnection(dst);

		result += juce::String("[MIDI Input]") + " - " + juce::String(dst) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addMixerTrackMidiOutput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		graph->setMIDITrk2OConnection(src);

		result += juce::String(src) + " - " + "[MIDI Output]" + "\n";
	}

	return CommandFuncResult{ true, result };
}

#include "../source/CloneableSourceManagerTemplates.h"

AUDIOCORE_FUNC(addAudioSource) {
	juce::String result;

	if (lua_isstring(L, 1) && !lua_isnumber(L, 1)) {
		juce::String sourcePath = juce::String::fromUTF8(lua_tostring(L, 1));

		CloneableSourceManager::getInstance()
			->createNewSourceThenLoadAsync<CloneableAudioSource>(sourcePath);
	}
	else {
		double sampleRate = luaL_checknumber(L, 1);
		int channelNum = luaL_checkinteger(L, 2);
		double length = luaL_checknumber(L, 3);

		CloneableSourceManager::getInstance()
			->createNewSource<CloneableAudioSource>(
				sampleRate, channelNum, length);
	}

	result += "Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addMIDISource) {
	juce::String result;

	if (lua_isstring(L, 1) && !lua_isnumber(L, 1)) {
		juce::String sourcePath = juce::String::fromUTF8(lua_tostring(L, 1));

		CloneableSourceManager::getInstance()
			->createNewSourceThenLoadAsync<CloneableMIDISource>(sourcePath);
	}
	else {
		CloneableSourceManager::getInstance()
			->createNewSource<CloneableMIDISource>();
	}

	result += "Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addSynthSource) {
	juce::String result;

	if (lua_isstring(L, 1) && !lua_isnumber(L, 1)) {
		juce::String sourcePath = juce::String::fromUTF8(lua_tostring(L, 1));

		CloneableSourceManager::getInstance()
			->createNewSourceThenLoadAsync<CloneableSynthSource>(sourcePath);
	}
	else {
		CloneableSourceManager::getInstance()
			->createNewSource<CloneableSynthSource>();
	}

	result += "Total Source Num: " + juce::String(CloneableSourceManager::getInstance()->getSourceNum()) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addSequencerTrack) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackTypeArg = luaL_checkinteger(L, 2);
		juce::AudioChannelSet trackType = getChannelSet(trackTypeArg);

		graph->insertSource(luaL_checkinteger(L, 1), trackType);

		result += "Add Sequencer Track: [" + juce::String(trackTypeArg) + "]" + trackType.getDescription() + "\n";
		result += "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addSequencerTrackMidiOutputToMixer) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int dst = luaL_checkinteger(L, 2);
		graph->setMIDISrc2TrkConnection(src, dst);

		result += juce::String(src) + " - " + juce::String(dst) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addSequencerTrackMidiOutputToInstr) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int dst = luaL_checkinteger(L, 2);
		graph->setMIDISrc2InstrConnection(src, dst);

		result += juce::String(src) + " - " + juce::String(dst) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addSequencerTrackOutput) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dst = luaL_checkinteger(L, 3);
		int dstc = luaL_checkinteger(L, 4);
		graph->setAudioSrc2TrkConnection(src, dst, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + juce::String(dst) + ", " + juce::String(dstc) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addSequencerSourceInstance) {
	juce::String result;

	int trackIndex = luaL_checkinteger(L, 1);
	int srcIndex = luaL_checkinteger(L, 2);
	double startTime = luaL_checknumber(L, 3);
	double endTime = luaL_checknumber(L, 4);
	double offset = luaL_checknumber(L, 5);

	AudioCore::getInstance()->addSequencerSourceInstance(trackIndex, srcIndex,
		startTime, endTime, offset);

	result += "Add Sequencer Source Instance [" + juce::String(trackIndex) + "] : [" + juce::String(srcIndex) + "]\n";
	result += "Total Sequencer Source Instance: " + juce::String(AudioCore::getInstance()->getSequencerSourceInstanceNum(trackIndex)) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addRecorderSourceInstance) {
	juce::String result;

	int srcIndex = luaL_checkinteger(L, 1);
	double offset = luaL_checknumber(L, 2);

	AudioCore::getInstance()->addRecorderSourceInstance(srcIndex, offset);

	result += "Add Recorder Source Instance [" + juce::String(srcIndex) + "]\n";
	result += "Total Recorder Source Instance: " + juce::String(AudioCore::getInstance()->getRecorderSourceInstanceNum()) + "\n";

	return CommandFuncResult{ true, result };
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
