#include "AudioCommand.h"

class CommandParser {
	CommandParser() = delete;

private:
	using CommandFuncResult = std::tuple<bool, juce::String>;
	using CommandFunc = std::function<CommandFuncResult(const juce::StringArray&)>;
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

	static CommandFuncResult searchThenDo(const FuncMap& funcMap, const juce::StringArray& command) {
		if (command.size() == 0) {
			return CommandFuncResult{ false, "Empty Command" };
		}

		auto itFunc = funcMap.find(command[0]);
		if (itFunc != funcMap.end()) {
			juce::StringArray nextCommand{ command };
			nextCommand.remove(0);
			return itFunc->second(nextCommand);
		}

		return CommandFuncResult{ false, "Invalid Command:" + command[0] };
	};

private:
	static CommandFuncResult echoFunc(const juce::StringArray& command) {
		return CommandFuncResult{ true, "Echo Test Result" };
	};

	static CommandFuncResult parse(const juce::StringArray& command) {
		FuncMap funcMap;
		funcMap["echo"] = CommandParser::echoFunc;

		return CommandParser::searchThenDo(funcMap, command);
	};

public:
	static const AudioCommand::CommandResult run(AudioCore* audioCore, const juce::String& command) {
		if (!audioCore) {
			return AudioCommand::CommandResult{ false, command, "Invalid Audio Core" };
		}

		auto result = CommandParser::parse(CommandParser::split(command));
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
