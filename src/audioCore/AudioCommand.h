#pragma once

#include <JuceHeader.h>
#include <lua.hpp>

class AudioCore;

class AudioCommand final : private juce::DeletedAtShutdown {
public:
	AudioCommand();

public:
	using CommandResult = std::tuple<bool, juce::String, juce::String>;
	using CommandCallback = std::function<void(const CommandResult&)>;

	const CommandResult processCommand(const juce::String& command);
	void processCommandAsync(const juce::String& command, CommandCallback callback);

private:
	std::unique_ptr<lua_State, std::function<void(lua_State*)>> cState = nullptr;

public:
	static AudioCommand* getInstance();

private:
	static AudioCommand* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCommand)
};
