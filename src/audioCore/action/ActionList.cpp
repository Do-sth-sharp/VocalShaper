#include "ActionList.h"

#include "../AudioCore.h"
#include "../misc/Device.h"
#include "../plugin/Plugin.h"
#include "../Utils.h"

ActionListDeviceAudio::ActionListDeviceAudio() {}

bool ActionListDeviceAudio::doAction() {
	juce::String result;

	auto& deviceTypes = Device::getInstance()->getAvailableDeviceTypes();
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

	this->output(result);
	return true;
}

ActionListDeviceMidi::ActionListDeviceMidi() {}

bool ActionListDeviceMidi::doAction() {
	juce::String result;

	auto midiInputList = Device::getAllMIDIInputDevices();
	auto midiOutputList = Device::getAllMIDIOutputDevices();

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

	this->output(result);
	return true;
}

ActionListPluginBlackList::ActionListPluginBlackList() {}

bool ActionListPluginBlackList::doAction() {
	juce::String result;

	result += "========================================================================\n";
	result += "Plugin Black List\n";
	result += "========================================================================\n";

	auto blackList = Plugin::getInstance()->getPluginBlackList();
	for (auto& s : blackList) {
		result += s + "\n";
	}
	result += "========================================================================\n";

	this->output(result);
	return true;
}

ActionListPluginSearchPath::ActionListPluginSearchPath() {}

bool ActionListPluginSearchPath::doAction() {
	juce::String result;

	result += "========================================================================\n";
	result += "Plugin Search Path\n";
	result += "========================================================================\n";

	auto searchPath = Plugin::getInstance()->getPluginSearchPath();
	for (auto& s : searchPath) {
		result += s + "\n";
	}
	result += "========================================================================\n";

	this->output(result);
	return true;
}

ActionListPlugin::ActionListPlugin() {}

bool ActionListPlugin::doAction() {
	auto listResult = Plugin::getInstance()->getPluginList();
	if (!std::get<0>(listResult)) {
		this->output("Searching Audio Plugin...");
		return true;
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

	this->output(result);
	return true;
}

#define PLUGIN_PARAM_CATEGORY_CASE(t) \
	case juce::AudioProcessorParameter::Category::t: \
		result += "    Category: " #t "\n"; \
		break

ActionListInstrParam::ActionListInstrParam(int instr)
	: instr(instr) {}

bool ActionListInstrParam::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(this->instr)) {
			if (auto instr = track->getInstrProcessor()) {
				juce::String result;

				auto& paramList = instr->getPluginParamList();

				result += "========================================================================\n";
				result += "Param List Of Instr [" + juce::String(this->instr) + "]\n";
				result += "========================================================================\n";

				for (int i = 0; i < paramList.size(); i++) {
					if (auto param = dynamic_cast<juce::HostedAudioProcessorParameter*>(paramList.getUnchecked(i))) {
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

				this->output(result);
				return true;
			}
		}
	}

	return false;
}

ActionListEffectParam::ActionListEffectParam(
	int track, int effect)
	: track(track), effect(effect) {}

bool ActionListEffectParam::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(this->track)) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(this->effect)) {
					juce::String result;

					auto& paramList = effect->getPluginParamList();

					result += "========================================================================\n";
					result += "Param List Of Effect [" + juce::String(this->track) + ", " + juce::String(this->effect) + "]\n";
					result += "========================================================================\n";

					for (int i = 0; i < paramList.size(); i++) {
						if (auto param = dynamic_cast<juce::HostedAudioProcessorParameter*>(paramList.getUnchecked(i))) {
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

					this->output(result);
					return true;
				}
			}
		}
	}

	return false;
}
