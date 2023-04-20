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

void regCommandList(lua_State* L) {
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listDeviceAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listDeviceMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listPluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listPluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, listPlugin);
}
