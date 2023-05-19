#include "CommandUtils.h"

AUDIOCORE_FUNC(listDeviceAudio) {
	juce::String result;

	auto& deviceTypes = getAudioDeviceManager(audioCore)->getAvailableDeviceTypes();
	for (auto& i : deviceTypes) {
		i->scanForDevices();
	}

	result += "========================================================================\n";
	result += "Audio Device List\n";
	result += "========================================================================\n";
	result += "Input Device:\n";
	result += "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
	for (auto& type : deviceTypes) {
		static int deviceTypeCount = 0;
		result += "    [Type " + juce::String(deviceTypeCount++) + "] " + type->getTypeName() + "\n";
		result += "------------------------------------------------------------------------\n";

		auto deviceList = type->getDeviceNames(true);
		for (auto& deviceName : deviceList) {
			static int deviceCount = 0;

			auto device = type->createDevice(juce::String(), deviceName);

			result += "        [" + juce::String(deviceCount++) + "] " + deviceName + "\n";
			result += "            Sample Rate: ";
			auto sampleRateList = device->getAvailableSampleRates();
			for (auto i : sampleRateList) {
				result += juce::String(i) + " ";
			}
			result += "\n";
			result += "            Buffer Size: ";
			auto bufferSizeList = device->getAvailableBufferSizes();
			for (auto i : bufferSizeList) {
				result += juce::String(i) + " ";
			}
			result += "\n";
			result += "            Bit Depth: " + juce::String(device->getCurrentBitDepth()) + "\n";
			result += "            Latency: " + juce::String(device->getInputLatencyInSamples()) + "\n";
			result += "            Channels: ";
			auto channelList = device->getInputChannelNames();
			for (int i = 0; i < channelList.size(); i++) {
				result += juce::String(i) + "." + channelList[i] + " ";
			}
			result += "\n";

			delete device;
		}
	}
	result += "========================================================================\n";
	result += "Output Device:\n";
	result += "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
	for (auto& type : deviceTypes) {
		static int deviceTypeCount = 0;
		result += "    [Type " + juce::String(deviceTypeCount++) + "] " + type->getTypeName() + "\n";
		result += "------------------------------------------------------------------------\n";

		auto deviceList = type->getDeviceNames(false);
		for (auto& deviceName : deviceList) {
			static int deviceCount = 0;

			auto device = type->createDevice(deviceName, juce::String());

			result += "        [" + juce::String(deviceCount++) + "] " + deviceName + "\n";
			result += "            Sample Rate: ";
			auto sampleRateList = device->getAvailableSampleRates();
			for (auto i : sampleRateList) {
				result += juce::String(i) + " ";
			}
			result += "\n";
			result += "            Buffer Size: ";
			auto bufferSizeList = device->getAvailableBufferSizes();
			for (auto i : bufferSizeList) {
				result += juce::String(i) + " ";
			}
			result += "\n";
			result += "            Bit Depth: " + juce::String(device->getCurrentBitDepth()) + "\n";
			result += "            Latency: " + juce::String(device->getOutputLatencyInSamples()) + "\n";
			result += "            Channels: ";
			auto channelList = device->getOutputChannelNames();
			for (int i = 0; i < channelList.size(); i++) {
				result += juce::String(i) + "." + channelList[i] + " ";
			}
			result += "\n";

			delete device;
		}
	}
	result += "========================================================================\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(listDeviceMIDI) {
	juce::String result;

	auto midiInputList = juce::MidiInput::getAvailableDevices();
	auto midiOutputList = juce::MidiOutput::getAvailableDevices();

	result += "========================================================================\n";
	result += "MIDI Device List\n";
	result += "========================================================================\n";
	result += "MIDI Input Devices:\n";
	for (int i = 0; i < midiInputList.size(); i++) {
		auto& device = midiInputList.getReference(i);
		result += "    [" + juce::String(i) + "] " + device.name + "\n";
		result += "         " + device.identifier + "\n";
	}
	result += "========================================================================\n";
	result += "MIDI Output Devices:\n";
	for (int i = 0; i < midiOutputList.size(); i++) {
		auto& device = midiOutputList.getReference(i);
		result += "    [" + juce::String(i) + "] " + device.name + "\n";
		result += "        " + device.identifier + "\n";
	}
	result += "========================================================================\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(listPluginBlackList) {
	juce::String result;

	result += "========================================================================\n";
	result += "Plugin Black List\n";
	result += "========================================================================\n";

	auto blackList = audioCore->getPluginBlackList();
	for (auto& s : blackList) {
		result += s + "\n";
	}
	result += "========================================================================\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(listPluginSearchPath) {
	juce::String result;

	result += "========================================================================\n";
	result += "Plugin Search Path\n";
	result += "========================================================================\n";

	auto searchPath = audioCore->getPluginSearchPath();
	for (auto& s : searchPath) {
		result += s + "\n";
	}
	result += "========================================================================\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(listPlugin) {
	auto listResult = audioCore->getPluginList();
	if (!std::get<0>(listResult)) {
		return CommandFuncResult{ true, "Searching Audio Plugin..." };
	}

	juce::String result;
	result += "========================================================================\n";
	result += "Plugin List\n";
	result += "========================================================================\n";

	auto list = std::get<1>(listResult).getTypes();
	for (int i = 0; i < list.size(); i++) {
		result += "[" + juce::String(i) + "]\n";
		result += "    name: " + list[i].name + "\n";
		result += "    descriptiveName: " + list[i].descriptiveName + "\n";
		result += "    pluginFormatName: " + list[i].pluginFormatName + "\n";
		result += "    category: " + list[i].category + "\n";
		result += "    manufacturerName: " + list[i].manufacturerName + "\n";
		result += "    version: " + list[i].version + "\n";
		result += "    fileOrIdentifier: " + list[i].fileOrIdentifier + "\n";
		result += "    lastFileModTime: " + list[i].lastFileModTime.toString(true, true, true, true) + "\n";
		result += "    lastInfoUpdateTime: " + list[i].lastInfoUpdateTime.toString(true, true, true, true) + "\n";
		result += "    uniqueId: " + juce::String::toHexString(list[i].uniqueId) + "\n";
		result += "    identifier: " + list[i].createIdentifierString() + "\n";
		result += "    isInstrument: " + juce::String(list[i].isInstrument ? "true" : "false") + "\n";
		result += "    numInputChannels: " + juce::String(list[i].numInputChannels) + "\n";
		result += "    numOutputChannels: " + juce::String(list[i].numOutputChannels) + "\n";
		result += "    hasSharedContainer: " + juce::String(list[i].hasSharedContainer ? "true" : "false") + "\n";
		result += "    hasARAExtension: " + juce::String(list[i].hasARAExtension ? "true" : "false") + "\n";
		result += "    \n";
	}

	result += "========================================================================\n";

	return CommandFuncResult{ true, result };
}

#define PLUGIN_PARAM_CATEGORY_CASE(t) \
	case juce::AudioProcessorParameter::Category::t: \
		result += "    Category: " #t "\n"; \
		break

AUDIOCORE_FUNC(listSequencerPluginParam) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int instrIndex = luaL_checkinteger(L, 1);

		auto instr = graph->getInstrumentProcessor(instrIndex);
		if (instr) {
			auto& paramList = instr->getPluginParamList();

			result += "========================================================================\n";
			result += "Param List Of Instr [" + juce::String(instrIndex) + "]\n";
			result += "========================================================================\n";

			for (int i = 0; i < paramList.size(); i++) {
				auto param = dynamic_cast<juce::HostedAudioProcessorParameter*>(paramList.getUnchecked(i));
				if (param) {
					result += "[" + juce::String(i) + "] " + param->getName(INT_MAX) + "\n";
					result += "    Value: " + param->getCurrentValueAsText() + "\n";
					result += "    Label: " + param->getLabel() + "\n";
					switch (param->getCategory()) {
						PLUGIN_PARAM_CATEGORY_CASE(genericParameter);
						PLUGIN_PARAM_CATEGORY_CASE(inputGain);
						PLUGIN_PARAM_CATEGORY_CASE(outputGain);
						PLUGIN_PARAM_CATEGORY_CASE(inputMeter);
						PLUGIN_PARAM_CATEGORY_CASE(outputMeter);
						PLUGIN_PARAM_CATEGORY_CASE(compressorLimiterGainReductionMeter);
						PLUGIN_PARAM_CATEGORY_CASE(expanderGateGainReductionMeter);
						PLUGIN_PARAM_CATEGORY_CASE(analysisMeter);
						PLUGIN_PARAM_CATEGORY_CASE(otherMeter);
					default:
						result += "    Category: " "undefined" "\n";
						break;
					}
					result += "    Num Steps: " + juce::String(param->getNumSteps()) + "\n";
					result += "    Possible States: " + param->getAllValueStrings().joinIntoString(", ") + "\n";
					result += "    Is Discrete : " + juce::String(param->isDiscrete() ? "true" : "false") + "\n";
					result += "    Is Boolean : " + juce::String(param->isBoolean() ? "true" : "false") + "\n";
					result += "    Is Orientation Inverted: " + juce::String(param->isOrientationInverted() ? "true" : "false") + "\n";
					result += "    Is Automatable: " + juce::String(param->isAutomatable() ? "true" : "false") + "\n";
					result += "    Is Meta Parameter: " + juce::String(param->isMetaParameter() ? "true" : "false") + "\n";
					result += "    Version Hint: " + juce::String(param->getVersionHint()) + "\n";
					
					if (auto ptr = dynamic_cast<juce::AudioProcessorParameterWithID*>(param)) {
						result += "    ID: " + ptr->getParameterID() + "\n";
					}
				}
			}

			result += "========================================================================\n";
		}
	}

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(listMixerPluginParam) {
	juce::String result;

	auto graph = audioCore->getGraph();
	if (graph) {
		int trackIndex = luaL_checkinteger(L, 1);
		int effectIndex = luaL_checkinteger(L, 2);

		auto track = graph->getTrackProcessor(trackIndex);
		if (track) {
			auto pluginDock = track->getPluginDock();
			if (pluginDock) {
				auto effect = pluginDock->getPluginProcessor(effectIndex);
				if (effect) {
					auto& paramList = effect->getPluginParamList();

					result += "========================================================================\n";
					result += "Param List Of Effect [" + juce::String(trackIndex) + ", " + juce::String(effectIndex) + "]\n";
					result += "========================================================================\n";

					for (int i = 0; i < paramList.size(); i++) {
						auto param = dynamic_cast<juce::HostedAudioProcessorParameter*>(paramList.getUnchecked(i));
						if (param) {
							result += "[" + juce::String(i) + "] " + param->getName(INT_MAX) + "\n";
							result += "    Value: " + param->getCurrentValueAsText() + "\n";
							result += "    Label: " + param->getLabel() + "\n";
							switch (param->getCategory()) {
								PLUGIN_PARAM_CATEGORY_CASE(genericParameter);
								PLUGIN_PARAM_CATEGORY_CASE(inputGain);
								PLUGIN_PARAM_CATEGORY_CASE(outputGain);
								PLUGIN_PARAM_CATEGORY_CASE(inputMeter);
								PLUGIN_PARAM_CATEGORY_CASE(outputMeter);
								PLUGIN_PARAM_CATEGORY_CASE(compressorLimiterGainReductionMeter);
								PLUGIN_PARAM_CATEGORY_CASE(expanderGateGainReductionMeter);
								PLUGIN_PARAM_CATEGORY_CASE(analysisMeter);
								PLUGIN_PARAM_CATEGORY_CASE(otherMeter);
							default:
								result += "    Category: " "undefined" "\n";
								break;
							}
							result += "    Num Steps: " + juce::String(param->getNumSteps()) + "\n";
							result += "    Possible States: " + param->getAllValueStrings().joinIntoString(", ") + "\n";
							result += "    Is Discrete : " + juce::String(param->isDiscrete() ? "true" : "false") + "\n";
							result += "    Is Boolean : " + juce::String(param->isBoolean() ? "true" : "false") + "\n";
							result += "    Is Orientation Inverted: " + juce::String(param->isOrientationInverted() ? "true" : "false") + "\n";
							result += "    Is Automatable: " + juce::String(param->isAutomatable() ? "true" : "false") + "\n";
							result += "    Is Meta Parameter: " + juce::String(param->isMetaParameter() ? "true" : "false") + "\n";
							result += "    Version Hint: " + juce::String(param->getVersionHint()) + "\n";

							if (auto ptr = dynamic_cast<juce::AudioProcessorParameterWithID*>(param)) {
								result += "    ID: " + ptr->getParameterID() + "\n";
							}
						}
					}

					result += "========================================================================\n";
				}
			}
		}
	}

	return CommandFuncResult{ true, result };
}

void regCommandList(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listDeviceAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listDeviceMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listPluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listPluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listPlugin);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listSequencerPluginParam);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listMixerPluginParam);
}
