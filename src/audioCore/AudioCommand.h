#pragma once

#include <JuceHeader.h>

#ifndef VOCALSHAPER_USE_AUDIO_COMMAND_ASYNC_THREAD
#define VOCALSHAPER_USE_AUDIO_COMMAND_ASYNC_THREAD 0
#endif // !VOCALSHAPER_USE_AUDIO_COMMAND_ASYNC_THREAD

class AudioCore;

class AudioCommand final {
public:
	AudioCommand() = delete;
	AudioCommand(AudioCore* parent);

public:
	using CommandResult = std::tuple<bool, juce::String, juce::String>;
	using CommandCallback = std::function<void(const CommandResult&)>;

	const CommandResult processCommand(const juce::String& command);
	void processCommandAsync(const juce::String& command, CommandCallback callback);

private:
	AudioCore* const parent = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCommand)
};
