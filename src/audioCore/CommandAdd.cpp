#include "CommandUtils.h"

AUDIOCORE_FUNC(addPluginBlackList) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin black list while searching plugin." };
	}

	audioCore->addToPluginBlackList(luaL_checkstring(L, 1));
	return CommandFuncResult{ true, "Add to plugin black list." };
}

AUDIOCORE_FUNC(addPluginSearchPath) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin search path while searching plugin." };
	}

	audioCore->addToPluginSearchPath(luaL_checkstring(L, 1));
	return CommandFuncResult{ true, "Add to plugin search path." };
}

AUDIOCORE_FUNC(addMixerTrack) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		juce::AudioChannelSet trackType;
		int trackTypeArg = luaL_checkinteger(L, 2);
		switch (trackTypeArg) {
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
			trackType = juce::AudioChannelSet::ambisonic(trackTypeArg - 100);
			break;
		default:
			trackType = juce::AudioChannelSet::stereo();
			break;
		}

		graph->insertTrack(luaL_checkinteger(L, 1), trackType);

		result += "Add Mixer Track: [" + juce::String(trackTypeArg) + "]" + trackType.getDescription() + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(addMixerTrackSend) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
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

	auto graph = audioCore->getGraph();
	if (graph) {
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

	auto graph = audioCore->getGraph();
	if (graph) {
		int src = luaL_checkinteger(L, 1);
		int srcc = luaL_checkinteger(L, 2);
		int dstc = luaL_checkinteger(L, 3);
		graph->setAudioTrk2OConnection(src, srcc, dstc);

		result += juce::String(src) + ", " + juce::String(srcc) + " - " + "[Device] " + juce::String(dstc) + "\n";
	}

	return CommandFuncResult{ true, result };
}

void regCommandAdd(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addPluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addPluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrackSend);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrackInputFromDevice);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, addMixerTrackOutput);
}