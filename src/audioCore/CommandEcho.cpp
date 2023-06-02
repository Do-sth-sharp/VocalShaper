#include "CommandUtils.h"

AUDIOCORE_FUNC(echoDeviceAudio) {
	juce::String result;

	auto setup = getAudioDeviceManager(audioCore)->getAudioDeviceSetup();
	auto currentDevice = getAudioDeviceManager(audioCore)->getCurrentAudioDevice();
	auto currentType = getAudioDeviceManager(audioCore)->getCurrentAudioDeviceType();

	result += "========================================================================\n";
	result += "Current Audio Device Information\n";
	result += "========================================================================\n";
	result += "Sample Rate: " + juce::String(currentDevice->getCurrentSampleRate()) + "\n";
	result += "Buffer Size: " + juce::String(currentDevice->getCurrentBufferSizeSamples()) + "\n";
	result += "Bit Depth: " + juce::String(currentDevice->getCurrentBitDepth()) + "\n";
	result += "Device Type: " + currentType + "\n";
	result += "========================================================================\n";
	result += "Input Device: " + setup.inputDeviceName + "\n";
	result += "Input Latency: " + juce::String(currentDevice->getInputLatencyInSamples()) + "\n";
	result += "Input Channels:\n";
	auto inputChannels = currentDevice->getInputChannelNames();
	auto activeInputChannels = currentDevice->getActiveInputChannels();
	for (int i = 0; i < inputChannels.size(); i++) {
		result += "    [" + juce::String(i) + "] " + inputChannels[i] + " - " + (activeInputChannels[i] ? "ON" : "OFF") + "\n";
	}
	result += "========================================================================\n";
	result += "Output Device: " + setup.outputDeviceName + "\n";
	result += "Output Latency: " + juce::String(currentDevice->getOutputLatencyInSamples()) + "\n";
	result += "Output Channels:\n";
	auto outputChannels = currentDevice->getOutputChannelNames();
	auto activeOutputChannels = currentDevice->getActiveOutputChannels();
	for (int i = 0; i < outputChannels.size(); i++) {
		result += "    [" + juce::String(i) + "] " + outputChannels[i] + " - " + (activeOutputChannels[i] ? "ON" : "OFF") + "\n";
	}
	result += "========================================================================\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoDeviceMIDI) {
	juce::String result;

	auto midiInputList = juce::MidiInput::getAvailableDevices();
	auto midiOutputDevice = getAudioDeviceManager(audioCore)->getDefaultMidiOutput();

	result += "========================================================================\n";
	result += "Current MIDI Device Information\n";
	result += "========================================================================\n";
	result += "MIDI Input Devices:\n";
	for (int i = 0; i < midiInputList.size(); i++) {
		auto& device = midiInputList.getReference(i);
		result += "    [" + juce::String(i) + "] " + device.name + " - " + (getAudioDeviceManager(audioCore)->isMidiInputDeviceEnabled(device.identifier) ? "ON" : "OFF") + "\n";
		result += "        " + device.identifier + "\n";
	}
	result += "========================================================================\n";
	result += "MIDI Output Device: " + (midiOutputDevice ? midiOutputDevice->getName() : "") + "\n";
	result += "MIDI Output Device ID: " + (midiOutputDevice ? midiOutputDevice->getIdentifier() : "") + "\n";
	result += "========================================================================\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoMixerInfo) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		result += "========================================================================\n";
		result += "Mixer\n";
		result += "========================================================================\n";

		int icn = graph->getTotalNumInputChannels();
		int ocn = graph->getTotalNumOutputChannels();

		result += "Total Input Channel: " + juce::String(icn) + "\n";
		result += "Total Output Channel: " + juce::String(ocn) + "\n";
		result += "Total Track: " + juce::String(graph->getTrackNum()) + "\n";

		result += "========================================================================\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoMixerTrack) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		result += "========================================================================\n";
		result += "Mixer Track List\n";
		result += "========================================================================\n";

		int trackNum = graph->getTrackNum();
		for (int i = 0; i < trackNum; i++) {
			if (i > 0) {
				result += "------------------------------------------------------------------------\n";
			}

			auto track = graph->getTrackProcessor(i);
			result += "[" + juce::String(i) + "]\n";
			result += "\tBus Type: " + track->getAudioChannelSet().getDescription() + "\n";
			result += "\tInput Bus: " + juce::String(track->getBusCount(true)) + "\n";
			result += "\tOutput Bus: " + juce::String(track->getBusCount(false)) + "\n";
			result += "\tInput Channel: " + juce::String(track->getTotalNumInputChannels()) + "\n";
			result += "\tOutput Channel: " + juce::String(track->getTotalNumOutputChannels()) + "\n";
			result += "\tMute: " + juce::String(track->getMute() ? "True" : "False") + "\n";
		}

		result += "========================================================================\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoMixerTrackInfo) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackNum = graph->getTrackNum();
		int trackId = luaL_checkinteger(L, 1);
		if (trackId < 0 || trackId >= trackNum) {
			return CommandFuncResult{ false, "Invalid track ID: " + juce::String(trackId) };
		}

		result += "========================================================================\n";
		result += "Mixer Track " + juce::String(trackId) + "\n";
		result += "========================================================================\n";

		if (auto track = graph->getTrackProcessor(trackId)) {
			result += "Bus Type: " + track->getAudioChannelSet().getDescription() + "\n";
			result += "\tInput Bus: " + juce::String(track->getBusCount(true)) + "\n";
			result += "\tOutput Bus: " + juce::String(track->getBusCount(false)) + "\n";
			result += "\tInput Channel: " + juce::String(track->getTotalNumInputChannels()) + "\n";
			result += "\tOutput Channel: " + juce::String(track->getTotalNumOutputChannels()) + "\n";
			result += "Mute: " + juce::String(track->getMute() ? "True" : "False") + "\n";
			result += "Gain: " + juce::String(track->getGain()) + "\n";
			result += "Pan: " + juce::String(track->getPan()) + "\n";
			result += "Slider: " + juce::String(track->getSlider()) + "\n";

			result += "------------------------------------------------------------------------\n";

			result += "Input From Track:\n";
			auto inputFromTrackConnectiones = graph->getTrackInputFromTrackConnections(trackId);
			for (int i = 0; i < inputFromTrackConnectiones.size(); i++) {
				auto connection = inputFromTrackConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<0>(connection)) + ", " + juce::String(std::get<1>(connection))
					+ " - "
					+ juce::String(std::get<2>(connection)) + ", " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Input From Source:\n";
			auto inputFromSequencerConnectiones = graph->getTrackInputFromSrcConnections(trackId);
			for (int i = 0; i < inputFromSequencerConnectiones.size(); i++) {
				auto connection = inputFromSequencerConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<1>(connection)) + " - " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Input From Instrument:\n";
			auto inputFromInstrumentConnectiones = graph->getTrackInputFromInstrConnections(trackId);
			for (int i = 0; i < inputFromInstrumentConnectiones.size(); i++) {
				auto connection = inputFromInstrumentConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<1>(connection)) + " - " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Input From Device:\n";
			auto inputFromDeviceConnectiones = graph->getTrackInputFromDeviceConnections(trackId);
			for (int i = 0; i < inputFromDeviceConnectiones.size(); i++) {
				auto connection = inputFromDeviceConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<1>(connection)) + " - " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "------------------------------------------------------------------------\n";

			result += "Output To Track:\n";
			auto outputToTrackConnectiones = graph->getTrackOutputToTrackConnections(trackId);
			for (int i = 0; i < outputToTrackConnectiones.size(); i++) {
				auto connection = outputToTrackConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<0>(connection)) + ", " + juce::String(std::get<1>(connection))
					+ " - "
					+ juce::String(std::get<2>(connection)) + ", " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "Output To Device:\n";
			auto outputToDeviceConnectiones = graph->getTrackOutputToDeviceConnections(trackId);
			for (int i = 0; i < outputToDeviceConnectiones.size(); i++) {
				auto connection = outputToDeviceConnectiones.getUnchecked(i);
				result += "\t[" + juce::String(i) + "] "
					+ juce::String(std::get<1>(connection)) + " - " + juce::String(std::get<3>(connection))
					+ "\n";
			}

			result += "------------------------------------------------------------------------\n";

			result += "Plugins:\n";
			if (auto pluginDock = track->getPluginDock()) {
				auto pluginList = pluginDock->getPluginList();
				for (int i = 0; i < pluginList.size(); i++) {
					auto pluginState = pluginList.getUnchecked(i);
					result += "\t[" + juce::String(i) + "] "
						+ std::get<0>(pluginState) + " - "
						+ juce::String(std::get<1>(pluginState) ? "ON" : "OFF") + "\n";
				}
			}
		}

		result += "========================================================================\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoMixerTrackGain) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			result += "Mixer Track Gain Value: <" + juce::String(trackIndex) + "> " + juce::String(track->getGain()) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoMixerTrackPan) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			result += "Mixer Track Pan Value: <" + juce::String(trackIndex) + "> " + juce::String(track->getPan()) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoMixerTrackSlider) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			result += "Mixer Track Slider Value: <" + juce::String(trackIndex) + "> " + juce::String(track->getSlider()) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoInstrParamValue) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int instrIndex = luaL_checkinteger(L, 1);
		int paramIndex = luaL_checkinteger(L, 2);

		if (auto instr = graph->getInstrumentProcessor(instrIndex)) {
			result += "Instr Param Value: [" + juce::String(paramIndex) + "] " + instr->getParamName(paramIndex) + " - " + juce::String(instr->getParamValue(paramIndex)) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoInstrParamDefaultValue) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int instrIndex = luaL_checkinteger(L, 1);
		int paramIndex = luaL_checkinteger(L, 2);

		if (auto instr = graph->getInstrumentProcessor(instrIndex)) {
			result += "Instr Param Default Value: [" + juce::String(paramIndex) + "] " + instr->getParamName(paramIndex) + " - " + juce::String(instr->getParamDefaultValue(paramIndex)) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoEffectParamValue) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int paramIndex = luaL_checkinteger(L, 3);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(effectIndex)) {
					result += "Effect Param Value: [" + juce::String(paramIndex) + "] " + effect->getParamName(paramIndex) + " - " + juce::String(effect->getParamValue(paramIndex)) + "\n";
				}
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoEffectParamDefaultValue) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int paramIndex = luaL_checkinteger(L, 3);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(effectIndex)) {
					result += "Effect Param Default Value: [" + juce::String(paramIndex) + "] " + effect->getParamName(paramIndex) + " - " + juce::String(effect->getParamDefaultValue(paramIndex)) + "\n";
				}
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoInstrParamCC) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int instrIndex = luaL_checkinteger(L, 1);
		int paramIndex = luaL_checkinteger(L, 2);

		auto instr = graph->getInstrumentProcessor(instrIndex);
		if (instr) {
			result += "Instr Param MIDI CC: [" + juce::String(paramIndex) + "] " + instr->getParamName(paramIndex) + " - MIDI CC " + juce::String(instr->getParamCCConnection(paramIndex)) + "\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoEffectParamCC) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int paramIndex = luaL_checkinteger(L, 3);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(effectIndex)) {
					result += "Effect Param MIDI CC: [" + juce::String(paramIndex) + "] " + effect->getParamName(paramIndex) + " - MIDI CC " + juce::String(effect->getParamCCConnection(paramIndex)) + "\n";
				}
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoInstrCCParam) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int instrIndex = luaL_checkinteger(L, 1);
		int CCIndex = luaL_checkinteger(L, 2);

		if (auto instr = graph->getInstrumentProcessor(instrIndex)) {
			int paramIndex = instr->getCCParamConnection(CCIndex);
			if (paramIndex > -1) {
				result += "Instr Param MIDI CC: MIDI CC " + juce::String(CCIndex) + " - [" + juce::String(paramIndex) + "] " + instr->getParamName(paramIndex) + "\n";
			}
			else {
				result += "Instr Param MIDI CC: MIDI CC " + juce::String(CCIndex) + " - Disabled\n";
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoEffectCCParam) {
	juce::String result;

	if (auto graph = audioCore->getGraph()) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);
		int CCIndex = luaL_checkinteger(L, 3);

		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(effectIndex)) {
					int paramIndex = effect->getCCParamConnection(CCIndex);
					if (paramIndex > -1) {
						result += "Effect Param MIDI CC: MIDI CC " + juce::String(CCIndex) + " - [" + juce::String(paramIndex) + "] " + effect->getParamName(paramIndex) + "\n";
					}
					else {
						result += "Effect Param MIDI CC: MIDI CC " + juce::String(CCIndex) + " - Disabled\n";
					}
				}
			}
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoSourceNum) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		result += "Total Source Num: " + juce::String(manager->getSourceNum()) + "\n";
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(echoSource) {
	juce::String result;

	if (auto manager = CloneableSourceManager::getInstance()) {
		int sourceIndex = luaL_checkinteger(L, 1);

		if (auto source = manager->getSource(sourceIndex)) {
			result += "========================================================================\n";
			result += "Source " + juce::String(sourceIndex) + "\n";
			result += "========================================================================\n";
			result += "ID: " + juce::String(source->getId()) + "\n";
			result += "Name: " + source->getName() + "\n";
			result += "Length: " + juce::String(source->getSourceLength()) + "\n";
			result += "SampleRate: " + juce::String(source->getSampleRate()) + "\n";
			if (auto ptr = dynamic_cast<CloneableAudioSource*>(source.getSource())) {
				result += "Type: Audio\n";
				result += "Source SampleRate: " + juce::String(ptr->getSourceSampleRate()) + "\n";
				result += "Channel Num: " + juce::String(ptr->getChannelNum()) + "\n";
			}
			else if (auto ptr = dynamic_cast<CloneableMIDISource*>(source.getSource())) {
				result += "Type: MIDI\n";
				result += "Track Num: " + juce::String(ptr->getTrackNum()) + "\n";
			}
			else {
				result += "Type: Unknown\n";
			}
			result += "========================================================================\n";
		}
	}

	return CommandFuncResult{ true, result };
}

void regCommandEcho(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoDeviceAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoDeviceMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerInfo);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrack);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrackInfo);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrackGain);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrackPan);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoMixerTrackSlider);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoInstrParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoInstrParamDefaultValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoEffectParamValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoEffectParamDefaultValue);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoInstrParamCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoEffectParamCC);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoInstrCCParam);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoEffectCCParam);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoSourceNum);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, echoSource);
}
