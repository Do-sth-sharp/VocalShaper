#include "AudioCommand.h"
#include "AudioCore.h"

class CommandParser {
	CommandParser() = delete;

private:
	using CommandFuncResult = std::tuple<bool, juce::String>;
	using CommandFunc = std::function<CommandFuncResult(AudioCore*, const juce::StringArray&)>;
	using FuncMap = std::map<juce::String, CommandFunc>;

	static juce::StringArray split(const juce::String& command) {
		juce::StringArray result;

		juce::String temp;
		for (auto c : command) {
			static bool quotedFlag = false;

			if (c == ' ' && !quotedFlag) {
				if (!temp.isEmpty()) {
					result.add(temp);
					temp.clear();
				}
				continue;
			}

			if (c == '\"') {
				quotedFlag = !quotedFlag;
				continue;
			}

			temp += c;
		}
		if (!temp.isEmpty()) {
			result.add(temp);
			temp.clear();
		}

		return result;
	};

	static CommandFuncResult searchThenDo(AudioCore* audioCore, const FuncMap& funcMap, const juce::StringArray& command) {
		if (command.size() == 0) {
			return CommandFuncResult{ false, "Empty Command" };
		}

		auto itFunc = funcMap.find(command[0]);
		if (itFunc != funcMap.end()) {
			juce::StringArray nextCommand{ command };
			nextCommand.remove(0);
			return itFunc->second(audioCore, nextCommand);
		}

		return CommandFuncResult{ false, "Invalid Command:" + command[0] };
	};

private:
	static CommandFuncResult echoDeviceAudioFunc(AudioCore* audioCore, const juce::StringArray& command) {
		juce::String result;

		auto setup = audioCore->audioDeviceManager->getAudioDeviceSetup();
		auto currentDevice = audioCore->audioDeviceManager->getCurrentAudioDevice();
		auto currentType = audioCore->audioDeviceManager->getCurrentAudioDeviceType();

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
		result += "========================================================================";

		return CommandFuncResult{ true, result };
	};

	static CommandFuncResult echoDeviceMIDIFunc(AudioCore* audioCore, const juce::StringArray& command) {
		juce::String result;
		
		auto midiInputList = juce::MidiInput::getAvailableDevices();
		auto midiOutputDevice = audioCore->audioDeviceManager->getDefaultMidiOutput();

		result += "========================================================================\n";
		result += "Current MIDI Device Information\n";
		result += "========================================================================\n";
		result += "MIDI Input Devices:\n";
		for (int i = 0; i < midiInputList.size(); i++) {
			auto& device = midiInputList.getReference(i);
			result += "    [" + juce::String(i) + "] " + device.name + " - " + (audioCore->audioDeviceManager->isMidiInputDeviceEnabled(device.identifier) ? "ON" : "OFF") + "\n";
			result += "        " + device.identifier + "\n";
		}
		result += "========================================================================\n";
		result += "MIDI Output Device: " + (midiOutputDevice ? midiOutputDevice->getName() : "") + "\n";
		result += "MIDI Output Device ID: " + (midiOutputDevice ? midiOutputDevice->getIdentifier() : "") + "\n";
		result += "========================================================================";

		return CommandFuncResult{ true, result };
	};

	static CommandFuncResult echoDeviceFunc(AudioCore* audioCore, const juce::StringArray& command) {
		FuncMap funcMap;
		funcMap["audio"] = CommandParser::echoDeviceAudioFunc;
		funcMap["midi"] = CommandParser::echoDeviceMIDIFunc;

		return CommandParser::searchThenDo(audioCore, funcMap, command);
	};

	static CommandFuncResult echoFunc(AudioCore* audioCore, const juce::StringArray& command) {
		FuncMap funcMap;
		funcMap["device"] = CommandParser::echoDeviceFunc;

		return CommandParser::searchThenDo(audioCore, funcMap, command);
	};

	static CommandFuncResult listDeviceAudioFunc(AudioCore* audioCore, const juce::StringArray& command) {
		juce::String result;

		auto& deviceTypes = audioCore->audioDeviceManager->getAvailableDeviceTypes();
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
		result += "========================================================================";

		return CommandFuncResult{ true, result };
	};

	static CommandFuncResult listDeviceMIDIFunc(AudioCore* audioCore, const juce::StringArray& command) {
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
		result += "========================================================================";

		return CommandFuncResult{ true, result };
	};

	static CommandFuncResult listDeviceFunc(AudioCore* audioCore, const juce::StringArray& command) {
		FuncMap funcMap;
		funcMap["audio"] = CommandParser::listDeviceAudioFunc;
		funcMap["midi"] = CommandParser::listDeviceMIDIFunc;

		return CommandParser::searchThenDo(audioCore, funcMap, command);
	};

	static CommandFuncResult listFunc(AudioCore* audioCore, const juce::StringArray& command) {
		FuncMap funcMap;
		funcMap["device"] = CommandParser::listDeviceFunc;

		return CommandParser::searchThenDo(audioCore, funcMap, command);
	};

	static CommandFuncResult setDeviceAudioFunc(AudioCore* audioCore, const juce::StringArray& command) {
		if (command.size() < 2) {
			return CommandFuncResult{ false, "Invalid operand or value count"};
		}
		
		juce::String result;

		//TODO

		return CommandFuncResult{ true, result };
	};

	static CommandFuncResult setDeviceMIDIFunc(AudioCore* audioCore, const juce::StringArray& command) {
		if (command.size() < 2) {
			return CommandFuncResult{ false, "Invalid operand or value count" };
		}
		
		juce::String result;

		//TODO

		return CommandFuncResult{ true, result };
	};

	static CommandFuncResult setDeviceFunc(AudioCore* audioCore, const juce::StringArray& command) {
		FuncMap funcMap;
		funcMap["audio"] = CommandParser::setDeviceAudioFunc;
		funcMap["midi"] = CommandParser::setDeviceMIDIFunc;

		return CommandParser::searchThenDo(audioCore, funcMap, command);
	};

	static CommandFuncResult setFunc(AudioCore* audioCore, const juce::StringArray& command) {
		FuncMap funcMap;
		funcMap["device"] = CommandParser::setDeviceFunc;

		return CommandParser::searchThenDo(audioCore, funcMap, command);
	};

	static CommandFuncResult parse(AudioCore* audioCore, const juce::StringArray& command) {
		FuncMap funcMap;
		funcMap["echo"] = CommandParser::echoFunc;
		funcMap["list"] = CommandParser::listFunc;
		funcMap["set"] = CommandParser::setFunc;

		return CommandParser::searchThenDo(audioCore, funcMap, command);
	};

public:
	static const AudioCommand::CommandResult run(AudioCore* audioCore, const juce::String& command) {
		if (!audioCore) {
			return AudioCommand::CommandResult{ false, command, "Invalid Audio Core" };
		}

		auto result = CommandParser::parse(audioCore, CommandParser::split(command));
		return AudioCommand::CommandResult{
			std::get<0>(result), command, std::get<1>(result) };
	};
};

class CommandAsyncThread : public juce::Thread {
	AudioCore* const parent = nullptr;
	const juce::String command;
	const AudioCommand::CommandCallback callback;

public:
	CommandAsyncThread(AudioCore* parent, const juce::String& command, AudioCommand::CommandCallback callback)
		: Thread("Audio Command:" + command), parent(parent), command(command), callback(callback) {};
	~CommandAsyncThread() {
		if (this->isThreadRunning()) {
			this->stopThread(3000);
		}
	};

public:
	static void call(AudioCore* parent, const juce::String& command, AudioCommand::CommandCallback callback) {
		(new CommandAsyncThread(parent, command, callback))->startThread();
	}

private:
	void run() override {
		auto result = CommandParser::run(this->parent, command);
		juce::MessageManager::callAsync([callback = this->callback, result] { callback(result); });

		juce::MessageManager::callAsync([this] { delete this; });
	};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandAsyncThread);
};

AudioCommand::AudioCommand(AudioCore* parent) 
	: parent(parent) {
	jassert(parent);
}

const AudioCommand::CommandResult AudioCommand::processCommand(const juce::String& command) {
	return CommandParser::run(this->parent, command);
}

void AudioCommand::processCommandAsync(const juce::String& command, AudioCommand::CommandCallback callback) {
	CommandAsyncThread::call(this->parent, command, callback);
}
