#pragma once

#include <JuceHeader.h>

class CommandManager final
	: public juce::ApplicationCommandManager,
	private juce::DeletedAtShutdown {
public:
	CommandManager() = default;

	void init();

public:
	static CommandManager* getInstance();
	static void releaseInstance();

private:
	static CommandManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandManager)
};
