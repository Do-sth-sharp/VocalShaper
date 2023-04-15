#include "AudioCommand.h"
#include "AudioCore.h"

class CommandBase : public juce::DeletedAtShutdown {
	JUCE_LEAK_DETECTOR(CommandBase);

protected:
	using CommandFuncResult = std::tuple<bool, juce::String>;
	virtual CommandFuncResult func(AudioCore*, lua_State*) = 0;

	static juce::AudioDeviceManager* getAudioDeviceManager(AudioCore* ac) {
		return ac->audioDeviceManager.get();
	};

	int interface(lua_State* L) {
		if (!L) { return 0; };
		AudioCore* ac = AudioCore::getInstance();
	
		auto result = this->func(ac, L);
	
		lua_pushboolean(L, std::get<0>(result));
		lua_setglobal(L, "sta");
		lua_pushstring(L, std::get<1>(result).toRawUTF8());
		lua_setglobal(L, "res");
		return 0;
	};
};

#define AUDIOCORE_FUNC(n) \
	class n##Class final : public CommandBase { \
		JUCE_LEAK_DETECTOR(n##Class); \
		\
	private: \
		CommandFuncResult func(AudioCore*, lua_State*) override; \
		static n##Class* instance; \
		static n##Class* getInstance(){ \
			return n##Class::instance ? n##Class::instance : (n##Class::instance = new n##Class()); \
		} \
		\
	public: \
		static int lFunc(lua_State* L) { \
			return n##Class::getInstance()->interface(L); \
		}; \
	}; \
	\
	n##Class* n##Class::instance = nullptr; \
	\
	CommandBase::CommandFuncResult n##Class::func(AudioCore* audioCore, lua_State* L)

#define LUA_PUSH_AUDIOCORE_FUNC(L, n) \
	lua_pushcfunction(L, n##Class::lFunc)

#define LUA_ADD_AUDIOCORE_FUNC(L, s, n) \
	lua_pushstring(L, s); \
	LUA_PUSH_AUDIOCORE_FUNC(L, n); \
	lua_settable(L, -3)

#define LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, n) LUA_ADD_AUDIOCORE_FUNC(L, #n, n)

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

AUDIOCORE_FUNC(setDeviceAudioType) {
	juce::String result;
	audioCore->setCurrentAudioDeviceType(luaL_checkstring(L, 1));

	result += "Current Audio Device Type: " + getAudioDeviceManager(audioCore)->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + audioCore->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + audioCore->getAudioOutputDeviceName() + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceAudioInput) {
	juce::String result;
	auto err = audioCore->setAudioInputDevice(luaL_checkstring(L, 1));
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Device Type: " + getAudioDeviceManager(audioCore)->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + audioCore->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + audioCore->getAudioOutputDeviceName() + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceAudioOutput) {
	juce::String result;
	auto err = audioCore->setAudioOutputDevice(luaL_checkstring(L, 1));
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Device Type: " + getAudioDeviceManager(audioCore)->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + audioCore->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + audioCore->getAudioOutputDeviceName() + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceAudioSampleRate) {
	juce::String result;
	auto err = audioCore->setAudioSampleRate(luaL_checknumber(L, 1));
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Sample Rate: " + juce::String(audioCore->getAudioSampleRate()) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceAudioBufferSize) {
	juce::String result;
	auto err = audioCore->setAudioBufferSize(luaL_checkinteger(L, 1));
	if (err.isNotEmpty()) {
		result += err;
		if (result.isNotEmpty() && result.getLastCharacter() != '\n') {
			result += "\n";
		}
	}

	result += "Current Audio Buffer Size: " + juce::String(audioCore->getAudioBufferSize()) + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceMIDIInput) {
	juce::String result;

	juce::String device = luaL_checkstring(L, 1);
	audioCore->setMIDIInputDeviceEnabled(device, lua_toboolean(L, 2));
	result += "MIDI Input Device: " + device + " - " + (audioCore->getMIDIInputDeviceEnabled(device) ? "ON" : "OFF") + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(setDeviceMIDIOutput) {
	juce::String result;

	juce::String device = luaL_checkstring(L, 1);
	audioCore->setMIDIOutputDevice(device);
	result += "MIDI Output Device: " + device + "\n";

	return CommandFuncResult{ true, result };
}

AUDIOCORE_FUNC(searchPlugin) {
	audioCore->clearPluginList();
	audioCore->getPluginList();

	return CommandFuncResult{ true, "Searching Audio Plugin..." };
}

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

AUDIOCORE_FUNC(removePluginBlackList) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin black list while searching plugin." };
	}

	audioCore->removeFromPluginBlackList(luaL_checkstring(L, 1));
	return CommandFuncResult{ true, "Remove from plugin black list." };
}

AUDIOCORE_FUNC(removePluginSearchPath) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't change plugin search path while searching plugin." };
	}

	audioCore->removeFromPluginSearchPath(luaL_checkstring(L, 1));
	return CommandFuncResult{ true, "Remove from plugin search path." };
}

AUDIOCORE_FUNC(clearPlugin) {
	if (audioCore->pluginSearchThreadIsRunning()) {
		return CommandFuncResult{ false, "Don't clear plugin list while searching plugin." };
	}

	audioCore->clearPluginTemporary();
	return CommandFuncResult{ true, "Clear plugin list." };
}

AudioCommand::AudioCommand() {
	/** Init Lua State */
	this->cState = std::unique_ptr<lua_State, std::function<void(lua_State*)>>(
		luaL_newstate(), lua_close);

	/** Load Functions */
	lua_newtable(this->cState.get());

	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), echoDeviceAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), echoDeviceMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), listDeviceAudio);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), listDeviceMIDI);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), listPluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), listPluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), listPlugin);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), setDeviceAudioType);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), setDeviceAudioInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), setDeviceAudioOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), setDeviceAudioSampleRate);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), setDeviceAudioBufferSize);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), setDeviceMIDIInput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), setDeviceMIDIOutput);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), searchPlugin);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), addPluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), addPluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), removePluginBlackList);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), removePluginSearchPath);
	LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(this->cState.get(), clearPlugin);

	lua_setglobal(this->cState.get(), "AudioCore");
}

const AudioCommand::CommandResult AudioCommand::processCommand(const juce::String& command) {
	/** Do Command */
	if (luaL_dostring(this->cState.get(), command.toStdString().c_str())) {
		return AudioCommand::CommandResult{ false, command, luaL_checkstring(this->cState.get(), -1)};
	}

	/** Check Result */
	bool state = false;
	juce::String res = "Bad command return value!";
	if (lua_getglobal(this->cState.get(), "sta") == LUA_TBOOLEAN) {
		state = lua_toboolean(this->cState.get(), -1);
	}
	if (lua_getglobal(this->cState.get(), "res") == LUA_TSTRING) {
		res = juce::String::fromUTF8(lua_tostring(this->cState.get(), -1));
	}
	lua_pop(this->cState.get(), 2);

	return AudioCommand::CommandResult{ state, command, res };
}

void AudioCommand::processCommandAsync(const juce::String& command, AudioCommand::CommandCallback callback) {
	auto asyncFunc = [command, callback] {
		auto result = AudioCommand::getInstance()->processCommand(command);
		juce::MessageManager::callAsync([callback, result] { callback(result); });
	};
	juce::MessageManager::callAsync(asyncFunc);
}

AudioCommand* AudioCommand::getInstance() {
	return AudioCommand::instance ? AudioCommand::instance : (AudioCommand::instance = new AudioCommand());
}

AudioCommand* AudioCommand::instance = nullptr;
