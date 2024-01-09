#pragma once

#include <JuceHeader.h>

class CommandManager final
	: public juce::ApplicationCommandManager,
	public juce::ChangeListener,
	private juce::DeletedAtShutdown {
public:
	CommandManager();
	~CommandManager();

	void init();

	void startListening();
	void stopListening();

private:
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

public:
	static CommandManager* getInstance();
	static void releaseInstance();

private:
	static CommandManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandManager)
};
